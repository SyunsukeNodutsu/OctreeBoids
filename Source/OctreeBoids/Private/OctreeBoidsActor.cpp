// Fill out your copyright notice in the Description page of Project Settings.


#include "OctreeBoidsActor.h"
#include "Kismet/KismetSystemLibrary.h"

#define FAST_BOID_DEBUG_DRAW false

//! コンストラクタ
//*****************************************************************************
AOctreeBoidsActor::AOctreeBoidsActor()
{
	PrimaryActorTick.bCanEverTick = true;
}

//! デストラクタ
//*****************************************************************************
AOctreeBoidsActor::~AOctreeBoidsActor()
{
	if (MyOctree)
	{
		MyOctree->Clear();
		delete MyOctree;
		MyOctree = nullptr;
	}
}

//! 開始
//*****************************************************************************
void AOctreeBoidsActor::BeginPlay()
{
	Super::BeginPlay();

	FVector size = FVector(100, 100, 100);

	MyOctree = new Octree(FBoxNode(FVector(0), size));

	for (int i = 0; i < NumBoids; i++)
	{
		float x = FMath::RandRange(-size.X, size.X);
		float y = FMath::RandRange(-size.Y, size.Y);
		float z = FMath::RandRange(-size.Z, size.Z);

		float theta = FMath::RandRange(0.0f, PI * 2);
		float phi = FMath::RandRange(0.0f, PI);
		float vx = FMath::Sin(phi) * FMath::Cos(theta);
		float vy = FMath::Sin(phi) * FMath::Sin(theta);
		float vz = FMath::Cos(phi);
		const FVector RandomVector = FVector(vx, vy, vz);
		const float initVelocity = 24;

		FPoint boid(FVector(x, y, z));
		boid.Velocity = RandomVector * initVelocity;
		Boids.Add(boid);

		MyOctree->Insert(FVector());
	}
}

//! 更新
//*****************************************************************************
void AOctreeBoidsActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (MyOctree)
	{
		UpdateBoids(DeltaTime);

		if (DrawBoxNode)
		{
			MyOctree->Visualize(GetWorld());
		}
	}
}

//! ボイド更新
//*****************************************************************************
void AOctreeBoidsActor::UpdateBoids(float DeltaTime)
{
	const float CosAngleThreshold = FMath::Cos(FMath::DegreesToRadians(NeighborAngleThreshold));

#if FAST_BOID_DEBUG_DRAW
	int cnt = 0;
#endif
	for (auto& boid : Boids)
	{
		FVector position = boid.Position;

		// 場外への移動を防ぐ
		FVector wallAvoidanceForce = CalculateWallAvoidanceForce(position, MyOctree->GetBoundary(), 10.0f);
		boid.Accel += wallAvoidanceForce;

		// ボイドの挙動
		FVector separation = FVector(0);
		FVector alignment = FVector(0);
		FVector cohesion = FVector(0);

		int numNeighbors = 0;

		// 近傍ボイドの検索
		TArray<FPoint> found;
		MyOctree->Query(FBoxNode(position, FVector(NeighborDist)), found);

		for (auto& otherBoid : found)
		{
			FVector other = otherBoid.Position;
			FVector to = other - position;
			FVector dir = to.GetSafeNormal();
			FVector fwd = boid.Velocity.GetSafeNormal();
			float prod = FVector::DotProduct(fwd, dir);
			float distance = FVector::Distance(position, other);

			// 角度と距離で近傍ボイドと判定
			if ((prod > CosAngleThreshold) && (distance <= NeighborDist))
			{
				separation += (position - other).GetSafeNormal();
				alignment += otherBoid.Velocity;
				cohesion += other;

				numNeighbors++;
#if FAST_BOID_DEBUG_DRAW
				if (cnt == 0)
				{
					UKismetSystemLibrary::DrawDebugPoint(GetWorld(), otherBoid.Position, 5, FColor::Purple);
				}
#endif
			}
		}

		if (numNeighbors > 0)
		{
			// 分離
			separation /= numNeighbors;
			boid.Accel += separation * SeparationWeight;

			// 整列
			alignment /= numNeighbors;
			boid.Accel += (alignment - boid.Velocity) * AlignmentWeight;

			// 結束
			cohesion /= numNeighbors;
			boid.Accel += (cohesion - position) * CohesionWeight;
		}

		boid.Velocity += boid.Accel * DeltaTime;

		float minSpeed = 6;
		if (boid.Velocity.Size() < minSpeed)
		{
			boid.Velocity = boid.Velocity.GetSafeNormal() * minSpeed;
		}

		boid.Position += boid.Velocity * DeltaTime;

		boid.Accel = FVector(0);

		// ボイドの描画
#if FAST_BOID_DEBUG_DRAW
		FColor color = (cnt == 0 ? FColor::Red : FColor::Green);
		UKismetSystemLibrary::DrawDebugPoint(GetWorld(), boid.Position, 4, color);
		if (cnt == 0)
		{
			UKismetSystemLibrary::DrawDebugSphere(GetWorld(), boid.Position, NeighborDist, 12, FColor::Red);
		}
		cnt++;
#else
		UKismetSystemLibrary::DrawDebugPoint(GetWorld(), boid.Position, 4, FColor::Green);
#endif
	}

	// 八分木更新
	MyOctree->Clear();
	for (auto& boid : Boids)
	{
		MyOctree->Insert(boid.Position);
	}
}

//! 壁回避計算
//*****************************************************************************
FVector AOctreeBoidsActor::CalculateWallAvoidanceForce(const FVector& position, const FBoxNode& boundary, float maxForce)
{
	FVector force(0, 0, 0);
	float distanceToLeft = position.X - (boundary.Center.X - boundary.Size.X);
	float distanceToRight = (boundary.Center.X + boundary.Size.X) - position.X;
	float distanceToTop = (boundary.Center.Y + boundary.Size.Y) - position.Y;
	float distanceToBottom = position.Y - (boundary.Center.Y - boundary.Size.Y);
	float distanceToFront = (boundary.Center.Z + boundary.Size.Z) - position.Z;
	float distanceToBack = position.Z - (boundary.Center.Z - boundary.Size.Z);

	float avoidanceDistance = 10.0f;

	if (distanceToLeft < avoidanceDistance)
	{
		force.X += maxForce * (avoidanceDistance - distanceToLeft) / avoidanceDistance;
	}
	if (distanceToRight < avoidanceDistance)
	{
		force.X -= maxForce * (avoidanceDistance - distanceToRight) / avoidanceDistance;
	}
	if (distanceToTop < avoidanceDistance)
	{
		force.Y -= maxForce * (avoidanceDistance - distanceToTop) / avoidanceDistance;
	}
	if (distanceToBottom < avoidanceDistance)
	{
		force.Y += maxForce * (avoidanceDistance - distanceToBottom) / avoidanceDistance;
	}
	if (distanceToFront < avoidanceDistance)
	{
		force.Z -= maxForce * (avoidanceDistance - distanceToFront) / avoidanceDistance;
	}
	if (distanceToBack < avoidanceDistance)
	{
		force.Z += maxForce * (avoidanceDistance - distanceToBack) / avoidanceDistance;
	}

	return force;
}

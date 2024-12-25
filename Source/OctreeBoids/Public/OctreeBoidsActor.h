// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Octree.h"
#include "GameFramework/Actor.h"
#include "OctreeBoidsActor.generated.h"

/**
 * 八分木を使用したボイドシミュレーション
 */
UCLASS()
class OCTREEBOIDS_API AOctreeBoidsActor : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Control")
	int NumBoids = 1000;

	UPROPERTY(EditAnywhere, Category = "Control")
	float SeparationWeight = 0.5f;

	UPROPERTY(EditAnywhere, Category = "Control")
	float AlignmentWeight = 0.34f;

	UPROPERTY(EditAnywhere, Category = "Control")
	float CohesionWeight = 0.16f;

	UPROPERTY(EditAnywhere, Category = "Control")
	float NeighborDist = 24.0f;

	UPROPERTY(EditAnywhere, Category = "Control")
	float NeighborAngleThreshold = 90.0f;

	UPROPERTY(EditAnywhere, Category = "Control")
	bool DrawBoxNode = false;

public:
	AOctreeBoidsActor();
	~AOctreeBoidsActor();
	void BeginPlay() override;
	void Tick(float DeltaTime) override;
	void UpdateBoids(float DeltaTime);
	FVector CalculateWallAvoidanceForce(const FVector& position, const FBoxNode& boundary, float maxForce);

private:
	Octree* MyOctree = nullptr;
	TArray<FPoint> Boids;

};

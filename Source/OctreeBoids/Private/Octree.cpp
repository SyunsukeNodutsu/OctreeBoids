// Fill out your copyright notice in the Description page of Project Settings.


#include "Octree.h"
#include "Kismet/KismetSystemLibrary.h"

//! コンストラクタ
//*****************************************************************************
Octree::Octree(FBoxNode boundary, int level, int capacity)
{
	Level = level;
	Boundary = boundary;
	Capacity = capacity;
}

//! 分割
//*****************************************************************************
void Octree::Subdivide()
{
	FVector center = Boundary.Center;
	FVector size = Boundary.Size / 2;

	TopFrontLeft	= new Octree(FBoxNode(FVector(center.X + size.X, center.Y - size.Y, center.Z + size.Z), size), Level + 1, Capacity);
	TopFrontRight	= new Octree(FBoxNode(FVector(center.X - size.X, center.Y - size.Y, center.Z + size.Z), size), Level + 1, Capacity);
	TopBackLeft		= new Octree(FBoxNode(FVector(center.X + size.X, center.Y + size.Y, center.Z + size.Z), size), Level + 1, Capacity);
	TopBackRight	= new Octree(FBoxNode(FVector(center.X - size.X, center.Y + size.Y, center.Z + size.Z), size), Level + 1, Capacity);

	BottomFrontLeft = new Octree(FBoxNode(FVector(center.X + size.X, center.Y - size.Y, center.Z - size.Z), size), Level + 1, Capacity);
	BottomFrontRight= new Octree(FBoxNode(FVector(center.X - size.X, center.Y - size.Y, center.Z - size.Z), size), Level + 1, Capacity);
	BottomBackLeft	= new Octree(FBoxNode(FVector(center.X + size.X, center.Y + size.Y, center.Z - size.Z), size), Level + 1, Capacity);
	BottomBackRight = new Octree(FBoxNode(FVector(center.X - size.X, center.Y + size.Y, center.Z - size.Z), size), Level + 1, Capacity);

	IsDivided = true;
}

//! 追加
//*****************************************************************************
void Octree::Insert(FVector point)
{
	if (!Boundary.Constains(point))
	{
		return;
	}
	if (Points.Num() < Capacity || Level >= 8)
	{
		Points.Add(FPoint(point));
	}
	else
	{
		if (!IsDivided)
		{
			Subdivide();
		}
		TopFrontLeft->Insert(point);
		TopFrontRight->Insert(point);
		TopBackLeft->Insert(point);
		TopBackRight->Insert(point);

		BottomFrontLeft->Insert(point);
		BottomFrontRight->Insert(point);
		BottomBackLeft->Insert(point);
		BottomBackRight->Insert(point);

		/*for (const auto& oldPoint : Points)
		{
			TopFrontLeft->Insert(oldPoint.Position);
			TopFrontRight->Insert(oldPoint.Position);
			TopBackLeft->Insert(oldPoint.Position);
			TopBackRight->Insert(oldPoint.Position);

			BottomFrontLeft->Insert(oldPoint.Position);
			BottomFrontRight->Insert(oldPoint.Position);
			BottomBackLeft->Insert(oldPoint.Position);
			BottomBackRight->Insert(oldPoint.Position);
		}
		Points.Empty();*/
	}
}

//! クリア
//*****************************************************************************
void Octree::Clear()
{
	Points.Empty();
	if (IsDivided)
	{
		TopFrontLeft->Clear();
		TopFrontRight->Clear();
		TopBackLeft->Clear();
		TopBackRight->Clear();

		BottomFrontLeft->Clear();
		BottomFrontRight->Clear();
		BottomBackLeft->Clear();
		BottomBackRight->Clear();

		delete TopFrontLeft;
		delete TopFrontRight;
		delete TopBackLeft;
		delete TopBackRight;

		delete BottomFrontLeft;
		delete BottomFrontRight;
		delete BottomBackLeft;
		delete BottomBackRight;

		TopFrontLeft = nullptr;
		TopFrontRight = nullptr;
		TopBackLeft = nullptr;
		TopBackRight = nullptr;

		BottomFrontLeft = nullptr;
		BottomFrontRight = nullptr;
		BottomBackLeft = nullptr;
		BottomBackRight = nullptr;

		IsDivided = false;
	}
}

//! 範囲内のポイントを取得(ボックス)
//*****************************************************************************
void Octree::Query(FBoxNode range, TArray<FPoint>& found)
{
	if (!Boundary.Intersects(range))
	{
		return;
	}
	for (const auto& point : Points)
	{
		if (range.Constains(point.Position))
		{
			found.Add(point);
		}
	}
	if (IsDivided)
	{
		TopFrontLeft->Query(range, found);
		TopFrontRight->Query(range, found);
		TopBackLeft->Query(range, found);
		TopBackRight->Query(range, found);

		BottomFrontLeft->Query(range, found);
		BottomFrontRight->Query(range, found);
		BottomBackLeft->Query(range, found);
		BottomBackRight->Query(range, found);
	}
}

//! 範囲内のポイントを取得(サークル)
//*****************************************************************************
void Octree::Query(FVector center, float radius, TArray<FPoint>& found)
{
}

//! ビジュアライズ
//*****************************************************************************
void Octree::Visualize(const UObject* wcObject)
{
	FColor color = FColor::White;

	FVector PointA = FVector(Boundary.Center.X - Boundary.Size.X, Boundary.Center.Y - Boundary.Size.Y, Boundary.Center.Z + Boundary.Size.Z);
	FVector PointB = FVector(Boundary.Center.X + Boundary.Size.X, Boundary.Center.Y - Boundary.Size.Y, Boundary.Center.Z + Boundary.Size.Z);
	FVector PointC = FVector(Boundary.Center.X + Boundary.Size.X, Boundary.Center.Y + Boundary.Size.Y, Boundary.Center.Z + Boundary.Size.Z);
	FVector PointD = FVector(Boundary.Center.X - Boundary.Size.X, Boundary.Center.Y + Boundary.Size.Y, Boundary.Center.Z + Boundary.Size.Z);

	FVector PointE = FVector(Boundary.Center.X - Boundary.Size.X, Boundary.Center.Y - Boundary.Size.Y, Boundary.Center.Z - Boundary.Size.Z);
	FVector PointF = FVector(Boundary.Center.X + Boundary.Size.X, Boundary.Center.Y - Boundary.Size.Y, Boundary.Center.Z - Boundary.Size.Z);
	FVector PointG = FVector(Boundary.Center.X + Boundary.Size.X, Boundary.Center.Y + Boundary.Size.Y, Boundary.Center.Z - Boundary.Size.Z);
	FVector PointH = FVector(Boundary.Center.X - Boundary.Size.X, Boundary.Center.Y + Boundary.Size.Y, Boundary.Center.Z - Boundary.Size.Z);

	UKismetSystemLibrary::DrawDebugLine(wcObject, PointA, PointB, color);
	UKismetSystemLibrary::DrawDebugLine(wcObject, PointB, PointC, color);
	UKismetSystemLibrary::DrawDebugLine(wcObject, PointC, PointD, color);
	UKismetSystemLibrary::DrawDebugLine(wcObject, PointD, PointA, color);

	UKismetSystemLibrary::DrawDebugLine(wcObject, PointE, PointF, color);
	UKismetSystemLibrary::DrawDebugLine(wcObject, PointF, PointG, color);
	UKismetSystemLibrary::DrawDebugLine(wcObject, PointG, PointH, color);
	UKismetSystemLibrary::DrawDebugLine(wcObject, PointH, PointE, color);

	UKismetSystemLibrary::DrawDebugLine(wcObject, PointA, PointE, color);
	UKismetSystemLibrary::DrawDebugLine(wcObject, PointB, PointF, color);
	UKismetSystemLibrary::DrawDebugLine(wcObject, PointC, PointG, color);
	UKismetSystemLibrary::DrawDebugLine(wcObject, PointD, PointH, color);

	if (IsDivided)
	{
		TopFrontLeft->Visualize(wcObject);
		TopFrontRight->Visualize(wcObject);
		TopBackLeft->Visualize(wcObject);
		TopBackRight->Visualize(wcObject);

		BottomFrontLeft->Visualize(wcObject);
		BottomFrontRight->Visualize(wcObject);
		BottomBackLeft->Visualize(wcObject);
		BottomBackRight->Visualize(wcObject);
	}

	/*for (const auto& point : Points)
	{
		UKismetSystemLibrary::DrawDebugString(wcObject, FVector(point.X, point.Y, -1),
			FString::Printf(TEXT("Lv: %d"), Level), nullptr, color);
	}*/
}

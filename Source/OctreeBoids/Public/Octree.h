// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 八分木に格納するポイント
 */
struct FPoint
{
	FPoint() = default;
	FPoint(FVector position)
	{
		Position = position;
	}

	FVector Position = FVector(0);

	// ボイドで使用
	FVector Velocity = FVector(0);
	FVector Accel = FVector(0);
};

/**
 * ボックス
 */
struct FBoxNode
{
	FBoxNode() = default;
	FBoxNode(FVector center, FVector size)
	{
		Center = center;
		Size = size;
	}
	bool Constains(FVector point) const
	{
		return (point.X > Center.X - Size.X && point.X < Center.X + Size.X &&
			point.Y > Center.Y - Size.Y && point.Y < Center.Y + Size.Y &&
			point.Z > Center.Z - Size.Z && point.Z < Center.Z + Size.Z);
	}
	bool Intersects(FBoxNode range) const
	{
		return !(range.Center.X - range.Size.X > Center.X + Size.X ||
			range.Center.X + range.Size.X < Center.X - Size.X ||
			range.Center.Y - range.Size.Y > Center.Y + Size.Y ||
			range.Center.Y + range.Size.Y < Center.Y - Size.Y ||
			range.Center.Z - range.Size.Z > Center.Z + Size.Z ||
			range.Center.Z + range.Size.Z < Center.Z - Size.Z);
	}
	FVector Center = FVector(0);
	FVector Size = FVector(0);
};

/**
 * 八分木
 */
class OCTREEBOIDS_API Octree
{
public:
	Octree(FBoxNode boundary, int level = 0, int capacity = 4);
	void Subdivide();
	void Insert(FVector point);
	void Clear();
	void Query(FBoxNode range, TArray<FPoint>& found);
	void Query(FVector center, float radius, TArray<FPoint>& found);
	void GetPoints(TArray<FPoint>& points) const { points = Points; }
	FBoxNode GetBoundary() const { return Boundary; }
	void Visualize(const UObject* wcObject);

private:
	int Capacity = 4;
	int Level = 0;
	bool IsDivided = false;
	FBoxNode Boundary;
	TArray<FPoint> Points;

	Octree* TopFrontLeft = nullptr;
	Octree* TopFrontRight = nullptr;
	Octree* TopBackLeft = nullptr;
	Octree* TopBackRight = nullptr;

	Octree* BottomFrontLeft = nullptr;
	Octree* BottomFrontRight = nullptr;
	Octree* BottomBackLeft = nullptr;
	Octree* BottomBackRight = nullptr;

};

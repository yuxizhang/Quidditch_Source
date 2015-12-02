// Fill out your copyright notice in the Description page of Project Settings.

#include "Quidditch_v1.h"
#include "MyFunc.h"
#define BoundaryX 15200.0/2
#define BoundaryY 5500.0/2

MyFunc::MyFunc()
{
}

MyFunc::~MyFunc()
{
}

bool MyFunc::OutofBoundary(FVector Location)
{
	if (Location.Z < 100) return true;
	if (Location.Z > 5000) return true;
	if (abs(Location.Y) > BoundaryY) return true;
	if (abs(Location.X) > BoundaryX) return true;
	return false;
}

FVector MyFunc::SetinBoundary(FVector Location)
{
	if (Location.Z < 100) Location.Z = 100;
	if (Location.Z > 5000) Location.Z = 5000;
	if (abs(Location.Y) > BoundaryY) Location.Y = (Location.Y > 0) ? BoundaryY : -BoundaryY;
	if (abs(Location.X) > BoundaryX) Location.X = (Location.X > 0) ? BoundaryX : -BoundaryX;
	return Location;
}

FRotator MyFunc::MapRotatorToPi(FRotator rotation)
{
	while (rotation.Yaw > 180) rotation.Yaw -= 360;
	while (rotation.Yaw < -180) rotation.Yaw += 360;
	while (rotation.Pitch > 180) rotation.Pitch -= 360;
	while (rotation.Pitch < -180) rotation.Pitch += 360;
	while (rotation.Roll > 180) rotation.Roll -= 360;
	while (rotation.Roll < -180) rotation.Roll += 360;
	return rotation;
}
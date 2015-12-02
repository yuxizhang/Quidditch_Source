// Fill out your copyright notice in the Description page of Project Settings.

#include "Quidditch_v1.h"
#include "GoldenSnitch.h"
#include <cmath>


#define AccMin 500.0
#define AccMax 1000.0
#define VelChangeInterval 1.0

// Sets default values
AGoldenSnitch::AGoldenSnitch()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AGoldenSnitch::BeginPlay()
{
	Super::BeginPlay();
	Velocity = FVector(500, 0, 0);
	TimeToChangeVel = VelChangeInterval;
}

// Called every frame
void AGoldenSnitch::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	FVector NewLocation = GetActorLocation();

	if (TimeToChangeVel < 0.0f)
	{

		Velocity = Rand3DVector(AccMin, AccMax);
		TimeToChangeVel += VelChangeInterval;
	}

	TimeToChangeVel -= DeltaTime;

	NewLocation += Velocity * DeltaTime;

	NewLocation = myFunc.SetinBoundary(NewLocation);
	
	
	/*
	if (NewLocation.Z < 10) NewLocation.Z = 10;
	if (NewLocation.Z > 5000) NewLocation.Z = 5000;
	if (abs(NewLocation.Y) > BoundaryY) NewLocation.Y = (NewLocation.Y > 0) ? BoundaryY : -BoundaryY;
	if (abs(NewLocation.X) > BoundaryX) NewLocation.X = (NewLocation.X > 0) ? BoundaryX : -BoundaryX;
*/
	SetActorLocation(NewLocation);
}

FVector AGoldenSnitch::Rand3DVector(double Min, double Max)
{
	double f = (double)rand() / RAND_MAX;
	double speed = Min + f * (Max - Min);
	double sqrspeed = speed * speed;
	FVector Velocity;
	double rz = (double)rand() / RAND_MAX * 0.5;
	double rx = (double)rand() / RAND_MAX * (1 - rz);
	double ry = 1 - abs(rz) - abs(rx);
	Velocity.X = sqrt(rx * sqrspeed);
	Velocity.Y = sqrt(ry * sqrspeed);
	Velocity.Z = sqrt(rz * sqrspeed);
	if ((double)rand() / RAND_MAX < 0.5) Velocity.X = 0 - Velocity.X;
	if ((double)rand() / RAND_MAX < 0.5) Velocity.Y = 0 - Velocity.Y;
	if ((double)rand() / RAND_MAX < 0.5) Velocity.Z = 0 - Velocity.Z;
	return Velocity;
}

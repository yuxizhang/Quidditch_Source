// Fill out your copyright notice in the Description page of Project Settings.

#include "Quidditch_v1.h"
#include "Quaffle.h"


// Sets default values
AQuaffle::AQuaffle()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	ID = 0;
}

// Called when the game starts or when spawned
void AQuaffle::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AQuaffle::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
	Direction.Normalize();
	FVector NewLocation = GetActorLocation() + Direction * Speed * DeltaTime;
	SetActorLocation(NewLocation);
	SetActorRotation(FRotator(0, 0, 0));
}

void AQuaffle::MoveTo(FVector location)
{
	
}
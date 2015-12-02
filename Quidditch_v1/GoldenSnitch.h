// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "MyFunc.h"
#include "GoldenSnitch.generated.h"


UCLASS()
class QUIDDITCH_V1_API AGoldenSnitch : public AActor
{
	GENERATED_BODY()
		FVector Rand3DVector(double Min, double Max);
	double TimeToChangeVel;
	MyFunc myFunc;

public:
	// Sets default values for this actor's properties
	AGoldenSnitch();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

	FVector Velocity;
};

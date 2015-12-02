// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "LeapMotionControllerActor.h"
#include "MyLeapMotionControllerActor.generated.h"

/**
 * 
 */
UCLASS()
class QUIDDITCH_V1_API AMyLeapMotionControllerActor : public ALeapMotionControllerActor
{
	GENERATED_BODY()

	bool isHoldingBall;
	float ThrowingDelay;
	FVector HandSpeed;

public:
	AMyLeapMotionControllerActor();

	virtual void Tick(float DeltaSeconds) override;
	
};

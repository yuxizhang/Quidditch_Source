// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Quaffle.generated.h"

UCLASS()
class QUIDDITCH_V1_API AQuaffle : public AActor
{
	GENERATED_BODY()

	UPROPERTY(Category = Mesh, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* PlaneMesh;



public:
	// Sets default values for this pawn's properties
	AQuaffle();

	UPROPERTY(Category = Attribute, EditAnywhere)
	float ControlRadius = 30;
	
	int ID;
	float Speed;
	FVector Direction;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	void MoveTo(FVector location);
};

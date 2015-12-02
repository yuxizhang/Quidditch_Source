// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Quaffle.h"
#include "GameFramework/Pawn.h"
#include "ChaserPawn.generated.h"


UCLASS()
class QUIDDITCH_V1_API AChaserPawn : public APawn
{
	GENERATED_BODY()

	/** StaticMesh component that will be the visuals for our flying pawn */
	UPROPERTY(Category = Mesh, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	
	class UStaticMeshComponent* PlaneMesh;



private:

	float TickTime;
	float YawOffset;
	const float BroomLength = 300;
	const float ArmLength = 100;
	float StayTime;


public:
	// Sets default values for this pawn's properties
	AChaserPawn();

	UPROPERTY(Category = Attribute, EditAnywhere)
	float MoveSpeed;

	UPROPERTY(Category = Attribute, EditAnywhere)
	float TurnSpeed;
	
	UPROPERTY(Category = Attribute, EditAnywhere)
	float MoveAccl;
	
	UPROPERTY(Category = Attribute, EditAnywhere)
	float TurnAccl;
	
	UPROPERTY(Category = Attribute, EditAnywhere)
	float ShootRange;

	bool isHoldingBall;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	virtual void NotifyHit(class UPrimitiveComponent* MyComp, class AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;

	void FlyToLocation(FVector TargetLocation, float DeltaTime);
	
	void TakeQuaffle(int id, float DeltaTime);
	void ThrowQuaffle(FVector dest, float speed);
	FVector GetQuaffleLocation(int id);
};

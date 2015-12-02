// Fill out your copyright notice in the Description page of Project Settings.

#include "Quidditch_v1.h"
#include "ChaserPawn.h"
#include "Engine.h"
#include "EngineUtils.h"
#include "AI/Navigation/NavigationPath.h"
#include "AI/Navigation/NavigationSystem.h"
#include "MyFunc.h"

// Sets default values
AChaserPawn::AChaserPawn()
{

	struct FConstructorStatics
	{
		ConstructorHelpers::FObjectFinderOptional<UStaticMesh> PlaneMesh;
		FConstructorStatics()
			: PlaneMesh(TEXT("/Game/Flying/Meshes/UFO.UFO"))
		{
		}
	};
	static FConstructorStatics ConstructorStatics;

	// Create static mesh component
	PlaneMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlaneMesh0"));
	PlaneMesh->SetStaticMesh(ConstructorStatics.PlaneMesh.Get());
	RootComponent = PlaneMesh;

 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	MoveSpeed = 100;
	TurnSpeed = 1;
	MoveAccl = 100;
	TurnAccl = 10;
	ShootRange = 2000;
}

// Called when the game starts or when spawned
void AChaserPawn::BeginPlay()
{
	Super::BeginPlay();
	isHoldingBall = false;
	TickTime = 0;
	YawOffset = 0;
	StayTime = 0;
}

// Called every frame
void AChaserPawn::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	if (StayTime > 0)
	{
		StayTime -= DeltaTime;
		return;
	}

	FVector CurrentLocation = this->GetActorLocation();
	FRotator CurrentRotation = this->GetActorRotation();


	//UNavigationPath* tpath;
	//UNavigationSystem* NavSys = UNavigationSystem::GetCurrent(GetWorld());

	FString text;

	FVector QuaffleLocaction = GetQuaffleLocation(0);
	float QuaffleDist = FVector::Dist(QuaffleLocaction, GetActorLocation());
	FVector GoalLocation(6500, 0, 1500);
	isHoldingBall = (QuaffleDist < ArmLength);
	YawOffset = 0;
	if (!isHoldingBall) {
		if (QuaffleDist < BroomLength && QuaffleDist > ArmLength) YawOffset = -5;
		FlyToLocation(QuaffleLocaction, DeltaTime);
	}
	else
	{
		float GoalDist = FVector::Dist(GoalLocation, GetActorLocation());
		if (GoalDist < ShootRange)
		{
			ThrowQuaffle(GoalLocation, 1000);
			StayTime = 3;
		}
		else
		{
			FlyToLocation(GoalLocation, DeltaTime);
			TakeQuaffle(0, DeltaTime);
		}
	}
}

// Called to bind functionality to input
void AChaserPawn::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);

}

void AChaserPawn::TakeQuaffle(int id, float DeltaTime)
{
	for (TActorIterator<AQuaffle> itr(GetWorld()); itr; ++itr)
	{
		if (itr->ID == id)
		{
			FVector NewLocation = itr->GetActorLocation() + GetActorForwardVector() * MoveSpeed * DeltaTime;
			itr->SetActorLocation(NewLocation);
			FString text;
			text = FString::Printf(TEXT("Taking Quaffle!"));
			GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Green, text);
			return;
		}
	}
	return;
}
void AChaserPawn::ThrowQuaffle(FVector dest, float speed)
{
	for (TActorIterator<AQuaffle> itr(GetWorld()); itr; ++itr)
	{
		itr->Speed = speed;
		itr->Direction = dest - itr->GetActorLocation();
		return;
	}
}


FVector AChaserPawn::GetQuaffleLocation(int id)
{
	for (TActorIterator<AQuaffle> itr(GetWorld()); itr; ++itr)
	{
		if (itr->ID == id)
		{
			FString text;
			text = FString::Printf(TEXT("Find Quaffle!"));
			GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Green, text);
			return itr->GetActorLocation();
		}
	}
	return FVector(-1, -1, -1);
}

void AChaserPawn::FlyToLocation(FVector TargetLocation, float DeltaTime)
{
	float er = 0.1;

	FVector CurrentLocation = GetActorLocation();
	FRotator CurrentRotation = GetActorRotation();
	FVector DeltaLocation = TargetLocation - CurrentLocation;
	FRotator TargetRotation = DeltaLocation.Rotation();
	//TargetRotation.Yaw = FMath::RadiansToDegrees(FMath::Atan(DeltaLocation.Y / (DeltaLocation.X + er)));
	//TargetRotation.Pitch = FMath::RadiansToDegrees(FMath::Atan(DeltaLocation.Z / FMath::Abs(DeltaLocation.X + er)));
	//TargetRotation.Roll = 0; // FMath::RadiansToDegrees(FMath::Atan(DeltaLocation.Y / (DeltaLocation.Z + er)));
	//
	//if (DeltaLocation.X < 0)
	//{
	//	TargetRotation.Yaw += 180;
	//	//TargetRotation.Pitch += 180;
	//}
	//if (DeltaLocation.Y < 0)
	//{
	//	//TargetRotation.Roll += 180;
	//}

	TargetRotation.Yaw += YawOffset;
	FRotator DeltaRotation = TargetRotation - CurrentRotation;
	DeltaRotation = MyFunc::MapRotatorToPi(DeltaRotation);

	FRotator NewRotation = CurrentRotation + DeltaRotation * TurnSpeed * DeltaTime;
	SetActorRotation(NewRotation);

	float Dist = FVector::Dist(TargetLocation, CurrentLocation);

	FVector NewLocation = CurrentLocation + GetActorForwardVector() * MoveSpeed * DeltaTime;
	SetActorLocation(NewLocation);

	FString text;
	text = FString::Printf(TEXT("Current Rotation: "));
	text += CurrentRotation.ToString();
	GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Green, text);

	text = FString::Printf(TEXT("Delta Rotation: "));
	text += DeltaRotation.ToString();
	GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Green, text);

	text = FString::Printf(TEXT("Target Rotation: "));
	text += TargetRotation.ToString();
	GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Green, text);

	text = FString::Printf(TEXT("Delta Location: "));
	text += DeltaLocation.ToString();
	GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Green, text);

	text = FString::Printf(TEXT("Target Location: "));
	text += TargetLocation.ToString();
	GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Green, text);


}

void AChaserPawn::NotifyHit(class UPrimitiveComponent* MyComp, class AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

	// Set velocity to zero upon collision
	//MoveSpeed = 0.f;
}
// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "Quidditch_v1.h"
#include "FlyingHMDPawn.h"
#include "Runtime/HeadMountedDisplay/Public/IHeadMountedDisplay.h"
#include "Engine.h"
#include "EngineUtils.h"
#include "LeapMotionControllerPlugin.h"
#include "LeapMotionDevice.h"
#include "Leap.h"
#include "LeapMotionTypes.h"
#include "Quaffle.h"
#include "MyFunc.h"

AFlyingHMDPawn::AFlyingHMDPawn()
{
	// Structure to hold one-time initialization
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

	// Create a spring arm component
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm0"));
	SpringArm->AttachTo(RootComponent);
	SpringArm->TargetArmLength = 160.0f; // The camera follows at this distance behind the character	
	SpringArm->SocketOffset = FVector(0.f,0.f,60.f);
	SpringArm->bEnableCameraLag = false;
	SpringArm->CameraLagSpeed = 15.f;

	// Create camera component 
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera0"));
	Camera->AttachTo(SpringArm, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false; // Don't rotate camera with controller

	// Set handling parameters
	Acceleration = 500.f;
	TurnSpeed = 100.f;
	MaxSpeed = 500.f;
	MinSpeed = 0.f;
	CurrentForwardSpeed = 100.f;
	
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;



}

void AFlyingHMDPawn::BeginPlay()
{
	if (GEngine->HMDDevice.IsValid() && GEngine->HMDDevice->IsHeadTrackingAllowed())
	{
		FQuat HMDOrientation;
		FVector HMDPosition;

		GEngine->HMDDevice->GetCurrentOrientationAndPosition(HMDOrientation, HMDPosition);

		HMDInitRotation = HMDOrientation.Rotator();
	}
}

void AFlyingHMDPawn::Tick(float DeltaSeconds)
{

	FLeapMotionDevice* Device = FLeapMotionControllerPlugin::GetLeapDeviceSafe();

	if (Device && Device->IsConnected())
	{
		Device->SetReferenceFrameOncePerTick();
		Leap::Frame frame = Device->Frame();

		FString leapLabel = FString::Printf(TEXT("LeapController - Frame: %u"), frame.id());
		GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Green, leapLabel);
		for (Leap::Hand hand : frame.hands())
		{
			if (hand.isLeft())
			{
				FString handLabel = FString::Printf(TEXT("    Hand %u: "), hand.id());
				handLabel += hand.palmPosition().toString().c_str();
				float yaw = hand.palmPosition().x;
				float pitch = hand.palmPosition().y;
				float acc = hand.palmPosition().z;

				AFlyingHMDPawn::MoveRightInput((yaw + 130) / 200.0f);
				AFlyingHMDPawn::ThrustInput(-acc / 100);
				AFlyingHMDPawn::MoveUpInput((pitch - 150) / 200.f);

				GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Green, handLabel);
			}
		}


	}
	else{
		// GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Green, FString(TEXT("LeapController - No Data")));
	}


	const FVector LocalMove = FVector(CurrentForwardSpeed * DeltaSeconds, 0.f, 0.f);

	// Move plan forwards (with sweep so we stop when we collide with things)
	AddActorLocalOffset(LocalMove, true);

	// Calculate change in rotation this frame
	FRotator DeltaRotation(0,0,0);
	DeltaRotation.Pitch = CurrentPitchSpeed * DeltaSeconds;
	DeltaRotation.Yaw = CurrentYawSpeed * DeltaSeconds;
	DeltaRotation.Roll = CurrentRollSpeed * DeltaSeconds;

	// Rotate plane
	AddActorLocalRotation(DeltaRotation);

	// Add HMD controlled camera rotation
	if (GEngine->HMDDevice.IsValid() && GEngine->HMDDevice->IsHeadTrackingAllowed())
	{
		FQuat HMDOrientation;
		FVector HMDPosition;



		// Disable bUpdateOnRT if using this method.
		GEngine->HMDDevice->GetCurrentOrientationAndPosition(HMDOrientation, HMDPosition);

		FRotator HMDViewRotation = HMDOrientation.Rotator();

		SpringArm->SetRelativeRotation(HMDViewRotation - HMDInitRotation);

	}
	
	// Call any parent class Tick implementation
	Super::Tick(DeltaSeconds);
}

void AFlyingHMDPawn::NotifyHit(class UPrimitiveComponent* MyComp, class AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

	// Set velocity to zero upon collision
	CurrentForwardSpeed = 0.f;
}


void AFlyingHMDPawn::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	check(InputComponent);

	// Bind our control axis' to callback functions
	InputComponent->BindAxis("Thrust", this, &AFlyingHMDPawn::ThrustInput);
	InputComponent->BindAxis("MoveUp", this, &AFlyingHMDPawn::MoveUpInput);
	InputComponent->BindAxis("MoveRight", this, &AFlyingHMDPawn::MoveRightInput);
}

void AFlyingHMDPawn::ThrustInput(float Val)
{
	// Is there no input?
	bool bHasInput = !FMath::IsNearlyEqual(Val, 0.f);
	/*Val = -Val / 50.0;
	bool bHasInput = Val > 0;*/
	// If input is not held down, reduce speed
	float CurrentAcc = bHasInput ? (Val * Acceleration) : (-0.5f * Acceleration);
	// Calculate new speed
	float NewForwardSpeed = CurrentForwardSpeed + (GetWorld()->GetDeltaSeconds() * CurrentAcc);
	// Clamp between MinSpeed and MaxSpeed
	CurrentForwardSpeed = FMath::Clamp(NewForwardSpeed, MinSpeed, MaxSpeed);
}

void AFlyingHMDPawn::MoveUpInput(float Val)
{
	bool bIsTurning = FMath::Abs(Val) > 0.2f;

	// Target pitch speed is based in input
	float TargetPitchSpeed = bIsTurning ? (Val * TurnSpeed) : 0; // (GetActorRotation().Pitch * -2.f);

	// Smoothly interpolate to target pitch speed
	CurrentPitchSpeed = FMath::FInterpTo(CurrentPitchSpeed, TargetPitchSpeed, GetWorld()->GetDeltaSeconds(), 2.f);

	if (bIsTurning && (abs(GetActorRotation().Pitch) > 89.0f) && (Val * GetActorRotation().Pitch > 0))
		CurrentPitchSpeed = 0;
}

void AFlyingHMDPawn::MoveRightInput(float Val)
{
	// Target yaw speed is based on input
	float TargetYawSpeed = (Val * TurnSpeed);

	// Smoothly interpolate to target yaw speed
	CurrentYawSpeed = FMath::FInterpTo(CurrentYawSpeed, TargetYawSpeed, GetWorld()->GetDeltaSeconds(), 2.f);

	// Is there any left/right input?
	const bool bIsTurning = FMath::Abs(Val) > 0.2f;

	// If turning, yaw value is used to influence roll
	// If not turning, roll to reverse current roll value
	float TargetRollSpeed = bIsTurning ? (CurrentYawSpeed * 0.5f) : (GetActorRotation().Roll * -2.f);

	// Smoothly interpolate roll speed
	CurrentRollSpeed = FMath::FInterpTo(CurrentRollSpeed, TargetRollSpeed, GetWorld()->GetDeltaSeconds(), 2.f);
}

//void AFlyingHMDPawn::TakeQuaffle(int id, float DeltaTime)
//{
//	for (TActorIterator<AQuaffle> itr(GetWorld()); itr; ++itr)
//	{
//		if (itr->ID == id)
//		{
//			FVector NewLocation = itr->GetActorLocation() + GetActorForwardVector() * MoveSpeed * DeltaTime;
//			itr->SetActorLocation(NewLocation);
//			FString text;
//			text = FString::Printf(TEXT("Taking Quaffle!"));
//			GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Green, text);
//			return;
//		}
//	}
//	return;
//}
//void AFlyingHMDPawn::ThrowQuaffle(FVector dest, float speed)
//{
//	for (TActorIterator<AQuaffle> itr(GetWorld()); itr; ++itr)
//	{
//		itr->Speed = speed;
//		itr->Direction = dest - itr->GetActorLocation();
//		return;
//	}
//}


FVector AFlyingHMDPawn::GetQuaffleLocation(int id)
{
	//for (TActorIterator<AQuaffle> itr(GetWorld()); itr; ++itr)
	//{
	//	if (itr->ID == id)
	//	{
	//		FString text;
	//		text = FString::Printf(TEXT("Find Quaffle!"));
	//		GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Green, text);
	//		return itr->GetActorLocation();
	//	}
	//}
	return FVector(-1, -1, -1);
}
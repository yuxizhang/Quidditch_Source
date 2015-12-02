// Fill out your copyright notice in the Description page of Project Settings.

#include "Quidditch_v1.h"
#include "MyLeapMotionControllerActor.h"
#include "Engine.h"
#include "EngineUtils.h"
#include "FlyingHMDPawn.h"
#include "Quaffle.h"
#include "LeapMotionControllerPlugin.h"
#include "LeapMotionDevice.h"
#include "Leap.h"
#include "LeapMotionTypes.h"

AMyLeapMotionControllerActor::AMyLeapMotionControllerActor()
{
	isHoldingBall = false;
	ThrowingDelay = 0;
}

void AMyLeapMotionControllerActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	//TActorIterator<AFlyingHMDPawn> Player(GetWorld());
	//FVector CameraLocation = Player->GetActorLocation();
	//FVector Offset(100, 0, 80);
	//FRotator PlayerRotation = Player->GetActorRotation();
	//FVector NewLocation = PlayerRotation.RotateVector(Offset) + CameraLocation;
	//FString text;
	//text = FString::Printf(TEXT("New Location: ")) + NewLocation.ToString();
	//GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Green, text);
	/*SetActorLocation(NewLocation);
	SetActorRotation(PlayerRotation);*/
	
	if (ThrowingDelay > 0) ThrowingDelay -= DeltaSeconds;
	

	FLeapMotionDevice* Device = FLeapMotionControllerPlugin::GetLeapDeviceSafe();
	if (Device && Device->IsConnected())
	{
		Device->SetHmdPolicy(false);
		Device->SetReferenceFrameOncePerTick();
		Leap::Frame frame = Device->Frame();

		FString leapLabel = FString::Printf(TEXT("LeapController - Frame: %u"), frame.id());
		GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Green, leapLabel);
		for (Leap::Hand hand : frame.hands())
		{
			if (hand.isRight())
			{
				FString text;
				TActorIterator<AQuaffle> Quaffle(GetWorld());
				FVector QuaffleLocation = Quaffle->GetActorLocation();
				FVector HandRelativeLocation = LEAPVECTOR_TO_FVECTOR(hand.palmPosition());
				FVector HandLocation = GetActorRotation().RotateVector(HandRelativeLocation) + GetActorLocation();

				text = "Hand Location: " + HandLocation.ToString();
				GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Red, text);
				int TipCount = 0;
				for (Leap::Finger finger : hand.fingers()){
					FString fingerLabel = FString::Printf(TEXT("        Finger %u: "), finger.id());
					FVector TipLocation = LEAPVECTOR_TO_FVECTOR(finger.tipPosition());
					TipLocation = GetActorRotation().RotateVector(TipLocation) + GetActorLocation();
					if (FVector::Dist(TipLocation, QuaffleLocation) < Quaffle->ControlRadius) TipCount++;
					fingerLabel += TipLocation.ToString();
					GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Green, fingerLabel);
				}
				text = FString::Printf(TEXT("Control Fingers: %d"), TipCount);
				GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Red, text);

				if (hand.pinchStrength() > 0)
					GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Red, "Pinching!");
				else
					GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Red, "Not Pinch");

				HandSpeed = GetActorRotation().RotateVector(LEAPVECTOR_TO_FVECTOR(hand.palmVelocity()));
				float speed = FVector::Dist(HandSpeed, FVector(0, 0, 0));
				text = FString::Printf(TEXT("Hand speed: %f"), speed);
				GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Red, text);

				if (TipCount >= 4 && hand.pinchStrength() > 0.3 && speed < 30 && ThrowingDelay <= 0) {
					FVector TargetLocation = GetActorRotation().RotateVector(HandRelativeLocation + FVector(8, 0, -5)) + GetActorLocation();
					Quaffle->SetActorLocation(TargetLocation);
					Quaffle->Speed = 0;
					isHoldingBall = true;
					
					text = FString::Printf(TEXT("Target Location: "));
					text += TargetLocation.ToString();
					GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Red, text);
				}
				else
				{
					if (isHoldingBall)
					{
						Quaffle->Speed = FVector::Dist(HandSpeed, FVector(0, 0, 0));
						Quaffle->Direction = HandSpeed;
						ThrowingDelay = 3;
					}
					isHoldingBall = false;
					
				}
			}
		}
	}
	else{
		GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Green, FString(TEXT("LeapController - No Data")));
	}
	
}
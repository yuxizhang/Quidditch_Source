// Fill out your copyright notice in the Description page of Project Settings.

#include "Quidditch_v1.h"
#include "MyGameMode.h"
#include "FlyingHMDPawn.h"

AMyGameMode::AMyGameMode()
{
	DefaultPawnClass = AFlyingHMDPawn::StaticClass();
	//PlayerControllerClass = AMyPlayerController::StaticClass();
}




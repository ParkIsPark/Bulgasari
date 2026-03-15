// Copyright Epic Games, Inc. All Rights Reserved.

#include "BulgasariGameMode.h"
#include "BulgasariCharacter.h"
#include "UObject/ConstructorHelpers.h"

ABulgasariGameMode::ABulgasariGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

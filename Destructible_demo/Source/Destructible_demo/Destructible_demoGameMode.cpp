// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "Destructible_demoGameMode.h"
#include "Destructible_demoCharacter.h"
#include "UObject/ConstructorHelpers.h"

ADestructible_demoGameMode::ADestructible_demoGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

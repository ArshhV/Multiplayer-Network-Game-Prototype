// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "Assignment2GameMode.h"
#include "Assignment2Character.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Assignment2GameState.h"

AAssignment2GameMode::AAssignment2GameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	//set the type of game state used in the game
	GameStateClass = AAssignment2GameState::StaticClass();

	// base decay rate
	DecayRate = 0.02f;

	// how frequently
	PowerDrainDelay = 0.25f;

	// base value for PowerToWinMultiplier
	PowerToWinMultiplier = 1.25f;

	//reset stats
	DeadPlayerCount = 0;
}

void AAssignment2GameMode::BeginPlay()
{
	GetWorldTimerManager().SetTimer(PowerDrainTimer, this, &AAssignment2GameMode::DrainPowerOverTime, PowerDrainDelay, true);

	//access world to get players
	UWorld* World = GetWorld();
	check(World);

	AAssignment2GameState* MyGameState = Cast<AAssignment2GameState>(GameState);
	check(MyGameState);

	//reset stats
	DeadPlayerCount = 0;

	//Transitioning the game to the playing state
	MyGameState->SetCurrentState(EPlayState::EPlaying);

	// go through all characters in game
	for (FConstControllerIterator It = World->GetControllerIterator(); It; ++It)
	{
		if (APlayerController* PlayerController = Cast<APlayerController>(*It))
		{
			if (AAssignment2Character* HealthCharacter = Cast<AAssignment2Character>(PlayerController->GetPawn()))
			{
				MyGameState->PowerToWin = HealthCharacter->GetInitialPower()*PowerToWinMultiplier;
				MyGameState->StaminaToWin = HealthCharacter->GetInitialStamina()*PowerToWinMultiplier;
				MyGameState->JoyToWin = HealthCharacter->GetInitialJoy()*PowerToWinMultiplier;
				break;
			}
		}
	}

}

float AAssignment2GameMode::GetDecayRate()
{
	return DecayRate;
}

float AAssignment2GameMode::GetPowerToWinMultiplier()
{
	return PowerToWinMultiplier;
}

void AAssignment2GameMode::DrainPowerOverTime()
{
	//access world to get players
	UWorld* World = GetWorld();
	check(World);

	AAssignment2GameState* MyGameState = Cast<AAssignment2GameState>(GameState);
	check(MyGameState);

	// go through all characters in game
	for (FConstControllerIterator It = World->GetControllerIterator(); It; ++It)
	{
		if (APlayerController* PlayerController = Cast<APlayerController>(*It))
		{
			if (AAssignment2Character* HealthCharacter = Cast<AAssignment2Character>(PlayerController->GetPawn()))
			{
				if (HealthCharacter->GetCurrentPower() > MyGameState->PowerToWin)
				{
					MyGameState->WinningPlayerName = HealthCharacter->GetName();
					MyGameState->SetCurrentState(EPlayState::EWon);
				}
				else if (HealthCharacter->GetCurrentPower() > 0 && HealthCharacter->GetCurrentStamina() > 0)
				{
					HealthCharacter->UpdatePower(-PowerDrainDelay * DecayRate*(HealthCharacter->GetInitialPower()));
					HealthCharacter->UpdateStamina(-PowerDrainDelay * DecayRate*(HealthCharacter->GetInitialStamina()));
					HealthCharacter->UpdateJoy(-PowerDrainDelay * DecayRate*(HealthCharacter->GetInitialJoy()));
				}
				else
				{
					HealthCharacter->DetachFromControllerPendingDestroy();
					//ded
					++DeadPlayerCount;

					// check if all ded
					if (DeadPlayerCount >= GetNumPlayers())
					{
						MyGameState->SetCurrentState(EPlayState::EGameOver);
					}
				}
			}
		}
	}
}

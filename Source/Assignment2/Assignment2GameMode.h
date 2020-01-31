// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Assignment2GameMode.generated.h"

UCLASS(minimalapi)
class AAssignment2GameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AAssignment2GameMode();

	//start the power drain timer
	virtual void BeginPlay() override;

	//get current rate of decay (% of initialPower per sec)
	UFUNCTION(BlueprintPure, Category = "Power")
	float GetDecayRate();

	//Get power level needed to win
	UFUNCTION(BlueprintPure, Category = "Power")
	float GetPowerToWinMultiplier();

protected:
	//delta time to update power and check game rules
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Power")
	float PowerDrainDelay;

	//timer for recurring power drain
	FTimerHandle PowerDrainTimer;

	//% of initialPower lost per second
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Power", Meta=(BlueprintProtected ="true"))
	float DecayRate;

	//power needed to win the game
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Power", Meta = (BlueprintProtected = "true"))
	float PowerToWinMultiplier;

	// track players out of power
	int32 DeadPlayerCount;

private:
	// drain power, update gameplay
	void DrainPowerOverTime();
};




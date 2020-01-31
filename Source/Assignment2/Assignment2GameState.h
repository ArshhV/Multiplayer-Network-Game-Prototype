// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Assignment2GameState.generated.h"

UENUM(BlueprintType)
enum EPlayState
{
	EPlaying,
	EGameOver,
	EWon,
	EUnknown
};

/**
 * 
 */
UCLASS()
class ASSIGNMENT2_API AAssignment2GameState : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	AAssignment2GameState();

	// required network scaffolding
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	//track power level required to win
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Power")
	float PowerToWin;

	//track stamina level required to win
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Power")
	float StaminaToWin;

	//track joy level required to win
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Power")
	float JoyToWin;

	// returns current state of gameplay
	UFUNCTION(BlueprintPure, Category = "Power")
	EPlayState GetCurrentState() const;

	//transition game to new play state. Only usable on server
	void SetCurrentState(EPlayState NewState);

	//Rep notify to allow client side changes based on state change
	UFUNCTION()
	void OnRep_CurrentState();

	//Name of player who won
	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Power")
	FString WinningPlayerName;

private:
	//track the current playing state
	UPROPERTY(ReplicatedUsing = OnRep_CurrentState)
	TEnumAsByte<enum EPlayState> CurrentState;

};

// Fill out your copyright notice in the Description page of Project Settings.

#include "Assignment2GameState.h"
#include "Net/UnrealNetwork.h"

AAssignment2GameState::AAssignment2GameState()
{
	//set default state
	CurrentState = EPlayState::EUnknown;
}

void AAssignment2GameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AAssignment2GameState, PowerToWin);
	DOREPLIFETIME(AAssignment2GameState, CurrentState);
	DOREPLIFETIME(AAssignment2GameState, WinningPlayerName);
	DOREPLIFETIME(AAssignment2GameState, StaminaToWin);
	DOREPLIFETIME(AAssignment2GameState, JoyToWin);
}

EPlayState AAssignment2GameState::GetCurrentState() const
{
	return CurrentState;
}

void AAssignment2GameState::SetCurrentState(EPlayState NewState)
{
	if (Role == ROLE_Authority)
	{
		CurrentState = NewState;
	}
}

void AAssignment2GameState::OnRep_CurrentState()
{

}

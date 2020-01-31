 // Fill out your copyright notice in the Description page of Project Settings.

#include "StaminaPickup.h"
#include "Assignment2.h"
#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"

AStaminaPickup::AStaminaPickup()
{
	//Keep movement synced from server to client
	bReplicateMovement = true;

	//This pickup is physics enabled and should move
	GetStaticMeshComponent()->SetSimulatePhysics(true);
	GetStaticMeshComponent()->SetMobility(EComponentMobility::Movable);

	// set a base value
	StaminaPower = 200.0f;
}

void AStaminaPickup::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AStaminaPickup, StaminaPower);
}

void AStaminaPickup::PickedUpBy(APawn * Pawn)
{
	Super::PickedUpBy(Pawn);

	if (Role == ROLE_Authority)
	{
		SetLifeSpan(0.1f);
	}
}

float AStaminaPickup::GetPower()
{
	return StaminaPower;
}

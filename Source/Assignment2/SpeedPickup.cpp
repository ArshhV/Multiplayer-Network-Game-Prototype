// Fill out your copyright notice in the Description page of Project Settings.

#include "SpeedPickup.h"
#include "Assignment2.h"
#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"

ASpeedPickup::ASpeedPickup()
{
	//Keep movement synced from server to client
	bReplicateMovement = true;

	//This pickup is physics enabled and should move
	GetStaticMeshComponent()->SetSimulatePhysics(true);
	GetStaticMeshComponent()->SetMobility(EComponentMobility::Movable);

	// set a base value
	SpeedPower = 1000.0f;
}

void ASpeedPickup::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASpeedPickup, SpeedPower);
}

void ASpeedPickup::PickedUpBy(APawn * Pawn)
{
	Super::PickedUpBy(Pawn);

	if (Role == ROLE_Authority)
	{
		SetLifeSpan(0.1f);
	}
}

float ASpeedPickup::GetPower()
{
	return SpeedPower;
}

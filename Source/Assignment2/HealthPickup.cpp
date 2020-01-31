// Fill out your copyright notice in the Description page of Project Settings.

#include "HealthPickup.h"
#include "Assignment2.h"
#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"

AHealthPickup::AHealthPickup()
{
	//Keep movement synced from server to client
	bReplicateMovement = true;

	//This pickup is physics enabled and should move
	GetStaticMeshComponent()->SetSimulatePhysics(true);
	GetStaticMeshComponent()->SetMobility(EComponentMobility::Movable);

	// set a base value
	HealthPower = 200.0f;
}

void AHealthPickup::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AHealthPickup, HealthPower);
}

void AHealthPickup::PickedUpBy(APawn * Pawn)
{
	Super::PickedUpBy(Pawn);

	if (Role == ROLE_Authority)
	{
		SetLifeSpan(0.1f);
	}
}

float AHealthPickup::GetPower()
{
	return HealthPower;
}

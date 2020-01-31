// Fill out your copyright notice in the Description page of Project Settings.

#include "SuperPackPickup.h"
#include "Assignment2.h"
#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"

ASuperPackPickup::ASuperPackPickup()
{
	//Keep movement synced from server to client
	bReplicateMovement = true;

	//This pickup is physics enabled and should move
	GetStaticMeshComponent()->SetSimulatePhysics(true);
	GetStaticMeshComponent()->SetMobility(EComponentMobility::Movable);

	// set a base value
	SuperPackPower = 750.0f;
}

void ASuperPackPickup::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASuperPackPickup, SuperPackPower);
}

void ASuperPackPickup::PickedUpBy(APawn * Pawn)
{
	Super::PickedUpBy(Pawn);

	if (Role == ROLE_Authority)
	{
		SetLifeSpan(0.1f);
	}
}

float ASuperPackPickup::GetPower()
{
	return SuperPackPower;
}

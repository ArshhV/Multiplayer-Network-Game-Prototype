// Fill out your copyright notice in the Description page of Project Settings.

#include "Pickup.h"
#include "Assignment2.h"
#include "Net/UnrealNetwork.h"
#include "Components/StaticMeshComponent.h"

APickup::APickup()
{
	// Tell UE4 to replicate this actor
	bReplicates = true;

	//Pickups do not need to tick every frame
	PrimaryActorTick.bCanEverTick = false;

	GetStaticMeshComponent()->SetGenerateOverlapEvents(true);

	if (Role == ROLE_Authority) 
	{
		bIsActive = true;
	}
}

void APickup::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APickup, bIsActive);
}

bool APickup::IsActive()
{
	return bIsActive;
}

void APickup::SetActive(bool NewPickupState)
{
	if (Role == ROLE_Authority)
	{
		bIsActive = NewPickupState;
	}
}

void APickup::OnRep_IsActive() 
{

}

void APickup::WasCollected_Implementation()
{
	// Log a debug message
	UE_LOG(LogClass, Log, TEXT("APickup::WasCollected_Implementation %s"), *GetName());
}

void APickup::PickedUpBy(APawn* Pawn)
{
	if (Role == ROLE_Authority)
	{
		//store the pawn who picked up the pick up
		PickupInstigator = Pawn;
		//notify clients about the pick up action
		ClientOnPickedUpBy(Pawn);
	}
}

void APickup::ClientOnPickedUpBy_Implementation(APawn* Pawn) 
{
	//store the pawn who picked up the pick up (client)
	PickupInstigator = Pawn;
	//blueprint native event, can't be replicated
	WasCollected();
}
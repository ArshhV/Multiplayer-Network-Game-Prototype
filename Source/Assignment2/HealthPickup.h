// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pickup.h"
#include "HealthPickup.generated.h"

/**
 * 
 */
UCLASS()
class ASSIGNMENT2_API AHealthPickup : public APickup
{
	GENERATED_BODY()
	
public:
	// Set default values for instances of this class
	AHealthPickup();

	// required network scaffolding
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	//Server Side handling of being picked up
	//UFUNCTION(BlueprintAuthorityOnly, Category = "Pickup")
	void PickedUpBy(APawn* Pawn) override;

	//public func to get power level
	float GetPower();

protected:
	//amount of power the health pickup holds
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category="Power", Meta = (BlueprintProtected="true"))
	float HealthPower;
};

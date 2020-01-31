// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpawnVolume.generated.h"

UCLASS()
class ASSIGNMENT2_API ASpawnVolume : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpawnVolume();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	//The spawn area where pickups will be created
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Spawning", Meta=(AllowPrivateAccess="true"))
	class UBoxComponent* WhereToSpawn;

	// Handle spawning new pickup
	void SpawnPickup();

	//time delay before spawning next pickup
	float SpawnDelay;

public:
	//Returns the spawn volume box component
	FORCEINLINE class UBoxComponent* GetWhereToSpawn() const { return WhereToSpawn; }

	//Find a random point within the box component
	UFUNCTION(BlueprintPure, Category="Spawning")
	FVector GetRandomPointInVolume();

protected:
	//This is the pickup to spawn 
	UPROPERTY(EditAnywhere, Category="Spawning")
	TSubclassOf<class APickup> WhatToSpawn;

	//Access to timer for recurring spawning
	FTimerHandle SpawnTimer;

	//Minimum spawn delay (seconds)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
	float SpawnDelayRangeLow;

	//Max spawn delay
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
	float SpawnDelayRangeHigh;
};

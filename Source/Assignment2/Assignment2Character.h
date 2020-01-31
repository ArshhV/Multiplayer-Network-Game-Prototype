// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Assignment2Character.generated.h"

UCLASS(config=Game)
class AAssignment2Character : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	/** Collection Sphere */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Health, meta = (AllowPrivateAccess = "true"))
	class USphereComponent* CollectionSphere;

public:
	AAssignment2Character();

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

protected:

	/** Resets HMD orientation in VR. */
	void OnResetVR();

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/** 
	 * Called via input to turn at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	/** Returns CollectionSphere subobject **/
	FORCEINLINE class USphereComponent* GetCollectionSphere() const { return CollectionSphere; }

	/******************************* MY CODE ********************************************************/
	/*******************************    ||   ********************************************************/
	/*******************************    \/   ********************************************************/

public:
	// required network scaffolding
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	//access character's starting power
	UFUNCTION(BlueprintPure, Category="Power")
	float GetInitialPower();

	//access character's starting stamina
	UFUNCTION(BlueprintPure, Category = "Power")
	float GetInitialStamina();

	//access character's starting Joy
	UFUNCTION(BlueprintPure, Category = "Power")
		float GetInitialJoy();

	//access character's current power
	UFUNCTION(BlueprintPure, Category = "Power")
	float GetCurrentPower();

	//access character's current stamina
	UFUNCTION(BlueprintPure, Category = "Power")
	float GetCurrentStamina();

	//access character's current Joy
	UFUNCTION(BlueprintPure, Category = "Power")
		float GetCurrentJoy();

	//access character's current Joy
	UFUNCTION(BlueprintPure, Category = "Power")
		FString GetCurrentPickup();

	// change power level using param DeltaPower
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Power")
	void UpdatePower(float DeltaPower);

	// change stamina level using param DeltaPower
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Power")
	void UpdateStamina(float DeltaPower);

	// change joy level using param DeltaPower
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Power")
	void UpdateJoy(float DeltaPower);

protected:
	//called on key press
	UFUNCTION(BlueprintCallable, Category = "Pickup")
		void CollectPickups();

	// called on server to process collection of pickups
	UFUNCTION(Reliable, Server, WithValidation)
	void ServerCollectPickups();

	// The character's starting power level
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Power", Meta = (BlueprintProtected = "true"))
	float InitialPower;

	// The character's starting power level
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Power", Meta = (BlueprintProtected = "true"))
	float InitialStamina;

	// The character's starting power level
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Power", Meta = (BlueprintProtected = "true"))
		float InitialJoy;

	//speed when power level is 0
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Power", Meta = (BlueprintProtected = "true"))
	float BaseSpeed;

	//multiplier to control current speed depending on the power level
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Power", Meta = (BlueprintProtected = "true"))
	float SpeedFactor;

	// update power visuals based on current power level
	UFUNCTION(BlueprintImplementableEvent, Category = "Power")
	void PowerChangeEffect();

private:
	// variable radius can be changed in editor
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category="Health", Meta=(AllowPrivateAccess="true"))
	float CollectionSphereRadius;

	// character's current power level during gameplay
	UPROPERTY(ReplicatedUsing = OnRep_CurrentPower, VisibleAnywhere, Category="Power")
	float CurrentPower;

	// character's current power level during gameplay
	UPROPERTY(ReplicatedUsing = OnRep_CurrentPower, VisibleAnywhere, Category = "Power")
	float CurrentStamina;

	// character's current power level during gameplay
	UPROPERTY(ReplicatedUsing = OnRep_CurrentPower, VisibleAnywhere, Category = "Power")
	float CurrentJoy;

	//power level updated on clients
	UFUNCTION()
	void OnRep_CurrentPower();

	//power level updated on clients
	UFUNCTION()
	void OnRep_CurrentStamina();

	//power level updated on clients
	UFUNCTION()
	void OnRep_CurrentJoy();

protected:
	//Pickup info for current pickup
	//class APickup* CurrentPickup;
	bool PickupFound = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pickup", Meta = (BlueprintProtected = "true"))
		FString PickupName;

	//Trace functions
	bool Trace(
		UWorld* World,
		TArray<AActor*>& ActorsToIgnore,
		const FVector& Start,
		const FVector& End,
		FHitResult& HitOut,
		ECollisionChannel CollisionChannel,
		bool ReturnPhysMat
	);

	void CallMyTrace();

	void ProcessTraceHit(FHitResult& HitOut);

	void ClearPickupInfo();

	virtual void Tick(float DeltaSeconds) override;

	// character's current power level during gameplay
	UPROPERTY(ReplicatedUsing = OnRep_CurrentPower, VisibleAnywhere, Category = "Pickup")
	class APickup* CurrentPickup;

	//power level updated on clients
	UFUNCTION()
	void OnRep_CurrentPickup();
};

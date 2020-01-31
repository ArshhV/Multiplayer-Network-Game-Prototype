// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "Assignment2Character.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Net/UnrealNetwork.h"
#include "Pickup.h"
#include "Components/SphereComponent.h"
#include "HealthPickup.h"
#include "StaminaPickup.h"
#include "SuperPackPickup.h"
#include "SpeedPickup.h"

//////////////////////////////////////////////////////////////////////////
// AAssignment2Character

AAssignment2Character::AAssignment2Character()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	//set default radius for collection sphere
	CollectionSphereRadius = 200.0f;
	//Create the collection sphere
	CollectionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollectionSphere"));
	CollectionSphere->AttachTo(RootComponent);
	CollectionSphere->SetSphereRadius(CollectionSphereRadius);

	//set base value for characters power
	InitialPower = 2000.0f;
	CurrentPower = InitialPower;

	//set base value for characters stamina
	InitialStamina = 2000.0f;
	CurrentStamina = InitialStamina;

	//set base value for characters joy
	InitialJoy = 2000.0f;
	CurrentJoy = InitialJoy;

	//set base values for controlling movement speed
	BaseSpeed = 10.0f;
	SpeedFactor = 0.75f;

	PickupName = "";

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
}

void AAssignment2Character::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AAssignment2Character, CollectionSphereRadius);
	DOREPLIFETIME(AAssignment2Character, InitialPower);
	DOREPLIFETIME(AAssignment2Character, CurrentPower);
	DOREPLIFETIME(AAssignment2Character, InitialStamina);
	DOREPLIFETIME(AAssignment2Character, CurrentStamina);
	DOREPLIFETIME(AAssignment2Character, InitialJoy);
	DOREPLIFETIME(AAssignment2Character, CurrentJoy);
	DOREPLIFETIME(AAssignment2Character, CurrentPickup);
}

float AAssignment2Character::GetInitialPower()
{
	return InitialPower;
}

float AAssignment2Character::GetCurrentPower()
{
	return CurrentPower;
}

float AAssignment2Character::GetInitialStamina()
{
	return InitialStamina;
}

float AAssignment2Character::GetCurrentStamina()
{
	return CurrentStamina;
}

float AAssignment2Character::GetInitialJoy()
{
	return InitialJoy;
}

float AAssignment2Character::GetCurrentJoy()
{
	return CurrentJoy;
}

FString AAssignment2Character::GetCurrentPickup()
{
	return PickupName;
}

void AAssignment2Character::UpdatePower(float DeltaPower)
{
	if (Role == ROLE_Authority)
	{
		//change current power
		CurrentPower += DeltaPower;

		//Fake Rep Notify (Listen server does not get rep notify automatically)
		OnRep_CurrentPower();

	}
}

void AAssignment2Character::UpdateStamina(float DeltaPower)
{
	if (Role == ROLE_Authority)
	{
		//change current stamina
		CurrentStamina += DeltaPower;

		//change current movement speed based on power level
		GetCharacterMovement()->MaxWalkSpeed = (BaseSpeed + SpeedFactor * CurrentStamina) - 8.0f;

		//Fake Rep Notify (Listen server does not get rep notify automatically)
		OnRep_CurrentStamina();

	}
}

void AAssignment2Character::UpdateJoy(float DeltaPower)
{
	if (Role == ROLE_Authority)
	{
		//change current stamina
		CurrentJoy += DeltaPower;

		//change current movement speed based on power level
		GetCharacterMovement()->MaxWalkSpeed = (BaseSpeed + SpeedFactor * CurrentStamina) - 2.0f;

		//Fake Rep Notify (Listen server does not get rep notify automatically)
		OnRep_CurrentJoy();
	}
}

//////////////////////////////////////////////////////////////////////////
// Input

void AAssignment2Character::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &AAssignment2Character::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AAssignment2Character::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AAssignment2Character::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AAssignment2Character::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &AAssignment2Character::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &AAssignment2Character::TouchStopped);

	// handle collecting pickups
	InputComponent->BindAction("CollectPickups", IE_Pressed, this, &AAssignment2Character::CollectPickups);

	// VR headset functionality
	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &AAssignment2Character::OnResetVR);
}

void AAssignment2Character::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void AAssignment2Character::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
		Jump();
}

void AAssignment2Character::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
		StopJumping();
}

void AAssignment2Character::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AAssignment2Character::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AAssignment2Character::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AAssignment2Character::MoveRight(float Value)
{
	if ( (Controller != NULL) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

void AAssignment2Character::CollectPickups()
{
	// ask server to collect pickups
	ServerCollectPickups();
}

void AAssignment2Character::OnRep_CurrentPower()
{
	PowerChangeEffect();
}

void AAssignment2Character::OnRep_CurrentStamina()
{
	PowerChangeEffect();
}

void AAssignment2Character::OnRep_CurrentJoy()
{
	PowerChangeEffect();
}

void AAssignment2Character::OnRep_CurrentPickup()
{
	PowerChangeEffect();
}

bool AAssignment2Character::ServerCollectPickups_Validate()
{
	return true;
}

void AAssignment2Character::ServerCollectPickups_Implementation()
{
	if (Role == ROLE_Authority)
	{
		//track the total power 
		float TotalPower = 0;
		float TotalStamina = 0;
		float TotalSuperPower = 0;
		float TotalSpeed = 0;

		//get all overlapping actors and store them in an array
		TArray<AActor*> CollectedActors;
		CollectionSphere->GetOverlappingActors(CollectedActors);

		//look at each actor inside the collection sphere
		for (int i = 0; i < CollectedActors.Num(); ++i)
		{
			//if its an valid and active pickup
			APickup* const TestPickup = Cast<APickup>(CollectedActors[i]);
			if (TestPickup != NULL && !TestPickup->IsPendingKill() && TestPickup->IsActive())
			{
				// add power if we found healthpack
				if (AHealthPickup* const TestHealth = Cast<AHealthPickup>(TestPickup))
				{
					TotalPower += TestHealth->GetPower();
				}
				// add power if we found staminapack
				else if (AStaminaPickup* const TestStamina = Cast<AStaminaPickup>(TestPickup))
				{
					TotalStamina += TestStamina->GetPower();
				}
				else if (ASuperPackPickup* const TestSuperPack = Cast<ASuperPackPickup>(TestPickup))
				{
					TotalSuperPower += TestSuperPack->GetPower();
				}
				else if (ASpeedPickup* const TestSpeed = Cast<ASpeedPickup>(TestPickup))
				{
					TotalSpeed += TestSpeed->GetPower();
				}
				//collect pickup and deactivate it
				TestPickup->PickedUpBy(this);
				TestPickup->SetActive(false);
			}
		}
		//Change characters stats based on what we picked up
		if (!FMath::IsNearlyZero(TotalPower, 0.001f))
		{
			UpdatePower(TotalPower);
			UpdateJoy(TotalPower);
		}
		if (!FMath::IsNearlyZero(TotalStamina, 0.001f))
		{
			UpdateStamina(TotalStamina);
			UpdateJoy(TotalStamina);
		}
		if (!FMath::IsNearlyZero(TotalSuperPower, 0.001f))
		{
			UpdatePower(TotalSuperPower);
			UpdateStamina(TotalSuperPower);
			UpdateJoy(TotalSuperPower);
		}
		if (!FMath::IsNearlyZero(TotalSpeed, 0.001f))
		{
			UpdateStamina(TotalSpeed);
		}
	}
}

bool AAssignment2Character::Trace(
	UWorld* World,
	TArray<AActor*>& ActorsToIgnore,
	const FVector& Start,
	const FVector& End,
	FHitResult& HitOut,
	ECollisionChannel CollisionChannel = ECC_Pawn,
	bool ReturnPhysMat = false
) {

	// The World parameter refers to our game world (map/level) 
	// If there is no World, abort
	if (!World)
	{
		return false;
	}

	// Set up our TraceParams object
	FCollisionQueryParams TraceParams(FName(TEXT("My Trace")), true, ActorsToIgnore[0]);

	// Should we simple or complex collision?
	TraceParams.bTraceComplex = true;

	// We don't need Physics materials 
	TraceParams.bReturnPhysicalMaterial = ReturnPhysMat;

	// Add our ActorsToIgnore
	TraceParams.AddIgnoredActors(ActorsToIgnore);

	// When we're debugging it is really useful to see where our trace is in the world
	// We can use World->DebugDrawTraceTag to tell Unreal to draw debug lines for our trace
	// (remove these lines to remove the debug - or better create a debug switch!)
	const FName TraceTag("MyTraceTag");
	World->DebugDrawTraceTag = TraceTag;
	TraceParams.TraceTag = TraceTag;


	// Force clear the HitData which contains our results
	HitOut = FHitResult(ForceInit);

	// Perform our trace
	World->LineTraceSingleByChannel
	(
		HitOut,		//result
		Start,	//start
		End, //end
		CollisionChannel, //collision channel
		TraceParams
	);

	// If we hit an actor, return true
	return (HitOut.GetActor() != NULL);
}

void AAssignment2Character::CallMyTrace()
{
	// Get the location of the camera (where we are looking from) and the direction we are looking in
	const FVector Start = FollowCamera->GetComponentLocation();
	const FVector ForwardVector = FollowCamera->GetForwardVector();

	// How for in front of our character do we want our trace to extend?
	// ForwardVector is a unit vector, so we multiply by the desired distance
	const FVector End = Start + ForwardVector * 512;

	// Force clear the HitData which contains our results
	FHitResult HitData(ForceInit);

	// What Actors do we want our trace to Ignore?
	TArray<AActor*> ActorsToIgnore;

	//Ignore the player character - so you don't hit yourself!
	ActorsToIgnore.Add(this);

	// Call our Trace() function with the paramaters we have set up
	// If it Hits anything
	if (Trace(GetWorld(), ActorsToIgnore, Start, End, HitData, ECC_Visibility, false))
	{
		// Process our HitData
		if (HitData.GetActor())
		{

			//UE_LOG(LogClass, Warning, TEXT("This a testing statement. %s"), *HitData.GetActor()->GetName());
			ProcessTraceHit(HitData);

		}
		else
		{
			// The trace did not return an Actor
			// An error has occurred
			// Record a message in the error log
		}
	}
	else
	{
		// We did not hit an Actor
		ClearPickupInfo();

	}

}

void AAssignment2Character::ProcessTraceHit(FHitResult& HitOut)
{

	// Cast the actor to APickup
	APickup* const TestPickup = Cast<APickup>(HitOut.GetActor());

	if (TestPickup)
	{
		// Keep a pointer to the Pickup
		CurrentPickup = TestPickup;

		// Set a local variable of the PickupName for the HUD
		//UE_LOG(LogClass, Warning, TEXT("PickupName: %s"), *TestPickup->GetPickupName());
		PickupName = TestPickup->GetName();

		PickupFound = true;
	}
	else
	{
		//UE_LOG(LogClass, Warning, TEXT("TestPickup is NOT a Pickup!"));
		ClearPickupInfo();
	}
}

void AAssignment2Character::ClearPickupInfo()
{
	CurrentPickup = NULL;
	PickupName = "";
	PickupFound = false;
}

void AAssignment2Character::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	//Remove comment to start line trace
	CallMyTrace();
}

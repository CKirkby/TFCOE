// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"


void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Gets the movement component and stores it for future use.
	InitialiseMovementComponent();
}

// Initialises the player input system
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (const APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			if (MappingContext)
			{
				Subsystem->AddMappingContext(MappingContext, 0);
			}
		}
	}

	// Initialises the action bindings 
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Insert Action bindings here

		// Binds the movement action to the input variables
		if (MoveAction)
		{
			EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerCharacter::MoveTrigger);
		}

		// Binds the interact action to the triggered function
		if (InteractAction)
		{
			EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &APlayerCharacter::InteractTrigger);
		}

		// Binds the sprint action to the trigger for starting, and the completed to the end function
		if (SprintAction)
		{
			EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &APlayerCharacter::SprintTrigger);
			EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &APlayerCharacter::SprintEnd);
			EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Canceled, this, &APlayerCharacter::SprintEnd);
		}
		
	} 
}

void APlayerCharacter::InitialiseMovementComponent()
{
	// Gets and caches the movement component for use 
	if (UCharacterMovementComponent* CharacterMovementComp = GetCharacterMovement())
	{
		MovementComponent = CharacterMovementComp;
	}
}

void APlayerCharacter::MoveTrigger(const FInputActionValue& Value)
{
	const FVector2D MovementVector = Value.Get<FVector2D>();
	
	// Movement for X vector
	AddMovementInput(FVector(1, 0, 0), MovementVector.X);

	// Movement for Y vector
	AddMovementInput(FVector(0, 1, 0), MovementVector.Y);
}

void APlayerCharacter::SprintTrigger()
{
	if (MovementComponent)
	{
		// Sets the movement speed to be the sprint speed on sprint start
		MovementComponent->MaxWalkSpeed = SprintSpeed;
		UE_LOG(LogTemp, Warning, TEXT("The new speed is: %1.0f"), MovementComponent->MaxWalkSpeed)
	}
}

void APlayerCharacter::SprintEnd()
{
	if (MovementComponent)
	{
		// Sets the movement speed to be the walk speed on sprint end
		MovementComponent->MaxWalkSpeed = WalkSpeed;
		UE_LOG(LogTemp, Warning, TEXT("The new speed is: %1.0f"), MovementComponent->MaxWalkSpeed)
	}
}

void APlayerCharacter::InteractTrigger()
{
	UE_LOG(LogTemp, Warning, TEXT("The player has interacted with something"))
}

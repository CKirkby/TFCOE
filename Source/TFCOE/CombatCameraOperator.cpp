// Created by Snow Paw Games


#include "CombatCameraOperator.h"

#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
ACombatCameraOperator::ACombatCameraOperator()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	
}

// Called when the game starts or when spawned
void ACombatCameraOperator::BeginPlay()
{
	Super::BeginPlay();

	InitialiseMovementComponent();
	
}

void ACombatCameraOperator::InitialiseMovementComponent()
{
	// Gets and caches the movement component for use 
	if (UCharacterMovementComponent* CharacterMovementComp = GetCharacterMovement())
	{
		MovementComponent = CharacterMovementComp;

		// Since this is a flying actor sets the movement mode to flying
		MovementComponent->SetMovementMode(MOVE_Flying);
	}
}

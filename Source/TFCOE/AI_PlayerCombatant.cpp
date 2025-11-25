// Created by Snow Paw Games


#include "AI_PlayerCombatant.h"
#include "GameFramework/Actor.h"
#include "AIController.h"

#include "CharacterCombatData.h"

// Sets default values
AAI_PlayerCombatant::AAI_PlayerCombatant()
{
	PrimaryActorTick.bCanEverTick = false;

	CombatData = CreateDefaultSubobject<UCharacterCombatData>(TEXT("Combat Data"));
}

void AAI_PlayerCombatant::BeginPlay()
{
	Super::BeginPlay();
}

void AAI_PlayerCombatant::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	GEngine->AddOnScreenDebugMessage(-1, 0.005f,FColor::Orange, TEXT("Current Time Points: " + FString::FromInt(CombatData->GetTimePoints())));

}

// An interface function to initiate movement 
void AAI_PlayerCombatant::NotifyMovementRequirementsMet(AActor* BoardPiece)
{
	if (!BoardPiece) return;

	if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(BoardPiece))
	{
		// Gets the target grid coordinates
		const FVector2D GridCoordinates = CombatInterface->GetGridCoordinates();

		// Performs a check to see if the player has sufficient time points to be able to move. Reducing the time points in the process
		if (CombatData->CheckCanAffordMovement(CombatData->GetCurrentGridCoordinates(), GridCoordinates))
		{
			CombatInterface->NotifyPieceClicked();

			if (AAIController* AI_Controller = Cast<AAIController>(GetController()))
			{
				// Moves the character. 
				const FVector MovementDestination = CombatInterface->GetBoardPieceLocation();
				AI_Controller->MoveToLocation(MovementDestination, 5.0f, false);
			}
		}
	}
}

// A interface function to set the current grid coordinates of this character.
void AAI_PlayerCombatant::SetCombatantCoordinates(const FVector2D Coordinates)
{
	CombatData->SetCurrentGridCoordinates(Coordinates);
}

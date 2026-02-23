// Created by Snow Paw Games


#include "AI_PlayerCombatant.h"
#include "GameFramework/Actor.h"
#include "AIController.h"

#include "CharacterCombatData.h"
#include "HealthComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AAI_PlayerCombatant::AAI_PlayerCombatant()
{
	PrimaryActorTick.bCanEverTick = true;

	CombatData = CreateDefaultSubobject<UCharacterCombatData>(TEXT("Combat Data"));
	HealthComp = CreateDefaultSubobject<UHealthComponent>(TEXT("Health Component"));
}

void AAI_PlayerCombatant::BeginPlay()
{
	Super::BeginPlay();

	InitialiseHealth();
}

void AAI_PlayerCombatant::InitialiseHealth() const
{
	// Gets the set health from the enemy configuration and inputs it into the health comp.
	HealthComp->InitialiseHealth(CombatData->GetHealthConfig());
}

void AAI_PlayerCombatant::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	GEngine->AddOnScreenDebugMessage(-1, 0.005f,FColor::Orange, TEXT("Current Time Points: " + FString::FromInt(CombatData->GetTimePoints())));
	
	GEngine->AddOnScreenDebugMessage(-1, 0.005f, FColor::Green, TEXT("Player Health: " + FString::FromInt(GetHealth())));
}

// An interface function to initiate movement 
void AAI_PlayerCombatant::NotifyMovementRequirementsMet(AActor* BoardPiece)
{
	if (!BoardPiece) return;

	if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(BoardPiece))
	{
		ICombatInterface* CombatInterfacePlayer = Cast<ICombatInterface>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
		if (!CombatInterfacePlayer) return;
		
		// Gets the Current and Target grid coordinates
		FIntPoint CurrentGridCoords = CombatInterfacePlayer->GetGridCoordinates();
		const FIntPoint TargetGridCoordinates = CombatInterface->GetGridCoordinates();

		TArray<FIntPoint> CalculatedPathway;
		if (CombatData->FindPathUsingAStar(CurrentGridCoords, TargetGridCoordinates, CalculatedPathway))
		{
			// Performs a check to see if the player has sufficient time points to be able to move. Reducing the time points in the process
			if (CombatData->CheckCanAffordMovement(CombatData->GetCurrentGridCoordinates(), TargetGridCoordinates))
			{
				CombatInterface->NotifyPieceClicked();
				
				CombatData->StartMovementAlongGridPath(CalculatedPathway);
			}
		}
		
	}
}

// A interface function to set the current grid coordinates of this character.
void AAI_PlayerCombatant::SetCombatantCoordinates(FIntPoint Coordinates)
{
	CombatData->SetCurrentGridCoordinates(Coordinates);
}

void AAI_PlayerCombatant::MoveAI_Character(const FVector Location)
{
	if (AAIController* AI_Controller = Cast<AAIController>(GetController()))
	{
		// Moves the character. 
		AI_Controller->MoveToLocation(Location, 5.0f, false);
	}
}

void AAI_PlayerCombatant::BeginTurnPhase()
{
	CombatData->ExecuteCurrentTurn();
}

void AAI_PlayerCombatant::SetAttackerReference(AActor* AttackerReference)
{
	if (AttackerReference && CombatData)
	{
		CombatData->SetPreviousAttacker(AttackerReference);
	}
}


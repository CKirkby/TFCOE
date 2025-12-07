// Created by Snow Paw Games

#include "MainGameMode.h"

#include "BoardManager.h"
#include "BoardPiece.h"
#include "CombatManager.h"

AMainGameMode::AMainGameMode()
{
	PrimaryActorTick.bCanEverTick = true;
	
	CombatManager = CreateDefaultSubobject<UCombatManager>(TEXT("Combat Manager"));
	BoardManager = CreateDefaultSubobject<UBoardManager>(TEXT("Board Manager"));
}

void AMainGameMode::BeginPlay()
{
	Super::BeginPlay();
}

void AMainGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	GEngine->AddOnScreenDebugMessage(-1, 0.005f,FColor::Red, TEXT("Current Turn: " + UEnum::GetValueAsString(CombatManager->GetCurrentTurnOrder())));
}

// Interface function to initialise the combat state
void AMainGameMode::InitialiseCombatState(const int CombatState) const
{
	CombatManager->SetCombatState(CombatState);
}

// Interface function to receive input on the players turn having ended.
void AMainGameMode::NotifyEndTurnTriggered()
{
	CombatManager->EndCurrentTurn();
}

// Interface to receive the current turn order. 
ETurnOrder AMainGameMode::GetCurrentTurnOrder()
{
	return CombatManager->GetCurrentTurnOrder();
}

// Interface call to trigger combat
void AMainGameMode::BeginCombat()
{
	InitialiseCombatState(1);
}

// Interface call to receive the board pieces to be stored for combat use
void AMainGameMode::InitialiseActiveBoard(TArray<AActor*> ActivePieces)
{
	for (AActor* BoardActor : ActivePieces)
	{
		if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(BoardActor))
		{
			const FVector2D Pos = CombatInterface->GetGridCoordinates();
			BoardManager->AddGridPairing(Pos, BoardActor);
		}
	}
}

// Interface call to receive the active combatants for combat use. 
void AMainGameMode::InitialiseActiveCombatants(TArray<AActor*> ActiveCombatants)
{
	if (!ActiveCombatants.IsEmpty())
	{
		CombatManager->SetActiveCombatants(ActiveCombatants);	
	}
}



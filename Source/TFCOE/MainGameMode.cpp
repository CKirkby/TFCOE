// Created by Snow Paw Games

#include "MainGameMode.h"

#include "BoardManager.h"
#include "BoardPiece.h"
#include "CombatManager.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"

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
void AMainGameMode::InitialiseCombatState(const int CombatState)
{
	// Sets the current state to be active combat
	CombatManager->SetCombatState(CombatState);
}

void AMainGameMode::MovePlayerToStartingPos()
{
	// Sets a timer to delay the movement slightly, this gives everything time to be setup properly. 
	FTimerHandle DelayBeforeMoveHandle;
	TWeakObjectPtr<AMainGameMode> SafeThis = this;
	GetWorld()->GetTimerManager().SetTimer(DelayBeforeMoveHandle, [SafeThis]
	{
		if (!SafeThis.IsValid()) return;

		// Gets the relevant interfaces I need for this function.
		ICombatInterface* CombatInterfaceBoardPiece = Cast<ICombatInterface>(SafeThis->BoardManager->GetCurrentStartingPiece());
		if (!CombatInterfaceBoardPiece) return;
		ICombatInterface* CombatInterfacePlayer = Cast<ICombatInterface>(UGameplayStatics::GetPlayerCharacter(SafeThis->GetWorld(), 0));
		if (!CombatInterfacePlayer) return;

		// Gets the location of the starting piece and then moves the player to that spot.
		const FVector LocationToMoveTo = CombatInterfaceBoardPiece->GetBoardPieceLocation();
		CombatInterfacePlayer->MoveAI_Character(LocationToMoveTo);
		
	}, 1.0f, false);
}

void AMainGameMode::MoveEnemiesToStartingPos(TMap<AActor*, ABoardPiece*> NewCombatants)
{
	if (NewCombatants.IsEmpty()) return;
	
	// Sets a timer to delay the movement slightly, this gives everything time to be setup properly. 
	FTimerHandle DelayBeforeMoveHandle;
	TWeakObjectPtr<AMainGameMode> SafeThis = this;
	GetWorld()->GetTimerManager().SetTimer(DelayBeforeMoveHandle, [SafeThis, NewCombatants]
	{
		if (!SafeThis.IsValid()) return;
		
		for (const auto CombatantPair : NewCombatants)
		{
			ABoardPiece* NewPiece = CombatantPair.Value;
			AActor* NewEnemy = CombatantPair.Key;
			if (!NewPiece || !NewEnemy) continue;
			
			// Gets the relevant interfaces I need for this function.
			ICombatInterface* CombatInterfaceBoardPiece = Cast<ICombatInterface>(NewPiece);
			ICombatInterface* CombatInterfaceEnemy = Cast<ICombatInterface>(NewEnemy);
			if (!CombatInterfaceEnemy || !CombatInterfaceBoardPiece) continue;

			// Gets the location of the starting piece and then moves the enemy to that spot.
			const FVector LocationToMoveTo = CombatInterfaceBoardPiece->GetBoardPieceLocation();
			CombatInterfaceEnemy->MoveAI_Character(LocationToMoveTo);
		}
	}, 1.0f, false);
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

// Interface call to get a grid peice in the board pairing from the coordinates.x
AActor* AMainGameMode::GetGridPieceFromCoordinates(const FVector2D Coordinates)
{
	return BoardManager->GetGridPiece(Coordinates);
}

// Interface call to trigger combat
void AMainGameMode::BeginCombat()
{
	InitialiseCombatState(1);
}

// Interface call to receive the board pieces to be stored for combat use
void AMainGameMode::InitialiseActiveBoard(TArray<AActor*> ActivePieces, AActor* StartingPiece)
{
	for (AActor* BoardActor : ActivePieces)
	{
		if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(BoardActor))
		{
			const FVector2D Pos = CombatInterface->GetGridCoordinates();
			BoardManager->AddGridPairing(Pos, BoardActor);

			if (StartingPiece)
			{
				BoardManager->SetCurrentStartingPiece(StartingPiece);
			}
		}
	}

	// Commands the player to move to the registered starting position.
	MovePlayerToStartingPos();
}

// Interface call to receive the active combatants for combat use. 
void AMainGameMode::InitialiseActiveCombatants(TMap<AActor*, ABoardPiece*> ActiveCombatants)
{
	if (ActiveCombatants.IsEmpty()) return;


	// Adds the active combatant to the combat manager for tracking
	for (const auto CombatantPair : ActiveCombatants)
	{
		AActor* NewActor = CombatantPair.Key;
		CombatManager->AddActiveCombatant(NewActor);
	}	

	// Commands the enemies to move to their starting positions.
	MoveEnemiesToStartingPos(ActiveCombatants);
}



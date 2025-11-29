// Created by Snow Paw Games

#include "MainGameMode.h"

#include "BoardManager.h"
#include "BoardPiece.h"
#include "CombatManager.h"
#include "Kismet/GameplayStatics.h"

AMainGameMode::AMainGameMode()
{
	PrimaryActorTick.bCanEverTick = true;
	
	CombatManager = CreateDefaultSubobject<UCombatManager>(TEXT("Combat Manager"));
	BoardManager = CreateDefaultSubobject<UBoardManager>(TEXT("Board Manager"));
}

void AMainGameMode::BeginPlay()
{
	Super::BeginPlay();

	// Testing for getting board actors
	TArray<AActor*> BoardActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABoardPiece::StaticClass(), BoardActors);

	for (AActor* BoardActor : BoardActors)
	{
		if (ABoardPiece* Piece = Cast<ABoardPiece>(BoardActor))
		{
			FVector2D Pos = Piece->GetGridPosition();
			BoardManager->AddGridPairing(Pos, Piece);
		}
	}
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

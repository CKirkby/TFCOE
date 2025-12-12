// Created by Snow Paw Games

#include "BoardManager.h"

UBoardManager::UBoardManager()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UBoardManager::BeginPlay()
{
	Super::BeginPlay();
}

void UBoardManager::AddGridPairing(const FVector2D GridCoordinates, AActor* BoardPiece)
{
	if (BoardPiece)
	{
		GridPairing.Add(GridCoordinates, BoardPiece);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Board Manager: Add Grid Pairing - Board Piece pointer failure"))
	}
}

AActor* UBoardManager::GetGridPiece(const FVector2D GridCoordinates) const
{
	if (AActor* FoundPiece = GridPairing.FindRef(GridCoordinates))
	{
		return FoundPiece;
	}
	UE_LOG(LogTemp, Error, TEXT("Board Manager: Get Board Piece - Cannot find pairing"))
	return nullptr;
}

void UBoardManager::ClearActiveBoard()
{
	GridPairing.Empty();

	CurrentStartingPiece = nullptr;
}

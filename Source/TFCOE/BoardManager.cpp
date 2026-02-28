// Created by Snow Paw Games

#include "BoardManager.h"

#include "BoardControllerInterface.h"
#include "BoardPiece.h"
#include "CombatInterface.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"

UBoardManager::UBoardManager()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UBoardManager::BeginPlay()
{
	Super::BeginPlay();
}

void UBoardManager::AddGridPairing(FIntPoint GridCoordinates, AActor* BoardPiece)
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

AActor* UBoardManager::GetGridPiece(FIntPoint GridCoordinates) const
{
	if (AActor* FoundPiece = GridPairing.FindRef(GridCoordinates))
	{
		return FoundPiece;
	}
	UE_LOG(LogTemp, Error, TEXT("Board Manager: Get Board Piece - Cannot find pairing"))
	return nullptr;
}

TArray<FIntPoint> UBoardManager::GetAllGridCoordinates() const
{
	TArray<FIntPoint> OutCoordinates;
	
	for (auto Pair : GridPairing)
	{
		OutCoordinates.Add(Pair.Key);
	}
	
	return OutCoordinates;
}

TArray<AActor*> UBoardManager::GetAllReachablePieces()
{
	// Gets the combat interface of the player. // TODO - Need to change this in future to get player or player party or current controllable character.
	ICombatInterface* CombatInterface_Player = Cast<ICombatInterface>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (!CombatInterface_Player || GridPairing.IsEmpty()) return {};
	
	//Gets the needed information
	FIntPoint CurrentCoordinates = CombatInterface_Player->GetGridCoordinates();
	int CurrentAP = CombatInterface_Player->GetTimePoints();
	
	// Initialises the temp variables
	TArray<FIntPoint> ReachableCoordinates;
	TQueue<FIntPoint> CheckQueue;
	TMap<FIntPoint, int32> DistanceMap;
	
	// Initialises the queue to start at current coords
	CheckQueue.Enqueue(CurrentCoordinates);
	DistanceMap.Add(CurrentCoordinates, 0);
	ReachableCoordinates.Add(CurrentCoordinates);
	
	// Declares the directions possible including diagonals
	const TArray<FIntPoint> Directions = 
	{
		{1, 0}, {-1, 0}, {0, 1}, {0, -1},
		{1, 1}, {1, -1}, {-1, 1}, {-1, -1}
	};
	
	while (!CheckQueue.IsEmpty())
	{
		FIntPoint CurrentCheck;
		CheckQueue.Dequeue(CurrentCheck);

		const int32 CurrentDist = DistanceMap[CurrentCheck];
		
		if (CurrentDist >= CurrentAP) continue;
		
		for (const auto& Dir : Directions)
		{
			FIntPoint NextCheck = CurrentCheck + Dir;
			
			if (!GridPairing.Contains(NextCheck)) continue; // Checks that the coordinates exist
			if (!IsGridPieceActive(NextCheck)) continue; // Checks that it is a walkable spot
			if (DistanceMap.Contains(NextCheck)) continue; // Checks if it has already been checked or not
			
			// Does a diagonal check.
			if (Dir.X != 0 && Dir.Y != 0)
			{
				const FIntPoint Adjacent1 = CurrentCheck + FIntPoint(Dir.X, 0);
				const FIntPoint Adjacent2 = CurrentCheck + FIntPoint(0, Dir.Y);
				
				// Checks to make sure the adjacent spots are active
				if (!IsGridPieceActive(Adjacent1) || !IsGridPieceActive(Adjacent2)) continue;
			}
			
			DistanceMap.Add(NextCheck, CurrentDist + 1);
			CheckQueue.Enqueue(NextCheck);
			ReachableCoordinates.Add(NextCheck);
		}
	}
	
	// After the coordinates are calculated, simply get the linked grid actors pieces.
	
	TArray<AActor*> ReachablePieces;
	for (auto ReachableCoordinate : ReachableCoordinates)
	{
		// Finds the linked grid piece and stores it
		if (AActor* FoundPiece = GetGridPiece(ReachableCoordinate))
		{
			ReachablePieces.Add(FoundPiece);
		}
	}
	
	// Removes the one the player is stood upon, as it is not needed to be visualised to them
	AActor* FoundCurrentPiece = GetGridPiece(CurrentCoordinates);
	if (FoundCurrentPiece && ReachablePieces.Contains(FoundCurrentPiece))
	{
		ReachablePieces.Remove(FoundCurrentPiece);
	}
	
	// Once  all grid pieces are found, return them.
	return ReachablePieces;
}

void UBoardManager::HighlightAllReachablePositions(const bool ActivateHighlight)
{
	// Highlights the specific pieces for movement. 
	CachedReachablePieces = GetAllReachablePieces();

	for (const auto Piece : CachedReachablePieces)
	{
		if (IBoardControllerInterface* BC_Interface = Cast<IBoardControllerInterface>(Piece))
		{
			if (ActivateHighlight)
			{
				// Notify individual piece of highlighting functionality inbound
				BC_Interface->NotifyHighlightBoardPiece(EHighlightType::Movement);
			}
			else
			{
				// Turns off the highlighted pieces manually.
				BC_Interface->NotifyHighlightBoardPiece(EHighlightType::None);
			}
		}
	}
}

void UBoardManager::ResetHighlightedPositions()
{
	// Resets all highlighted pieces.
	if (CachedReachablePieces.IsEmpty()) return;

	for (const auto Piece : CachedReachablePieces)
	{
		if (IBoardControllerInterface* BC_Interface = Cast<IBoardControllerInterface>(Piece))
		{
			BC_Interface->NotifyHighlightBoardPiece(EHighlightType::None);		
		}
	}
}

void UBoardManager::ClearActiveBoard()
{
	GridPairing.Empty();

	CurrentStartingPiece = nullptr;
}

bool UBoardManager::DoesGridCoordinatesExist(const FIntPoint Coords) const
{
	return GridPairing.Contains(Coords);
}

bool UBoardManager::IsGridPieceActive(const FIntPoint GridCoordinates) const
{
	AActor* FoundPiece = GetGridPiece(GridCoordinates);
	if (!FoundPiece) return false;
	
	// Gets its interface.
	ICombatInterface* CombatInterfaceGridPiece = Cast<ICombatInterface>(FoundPiece);
	if (!CombatInterfaceGridPiece) return false;

	EPieceState State = CombatInterfaceGridPiece->GetCurrentPieceState();
	
	// Checks the piece isn't occupied or non-walkable.
	return State != Occupied && State != Disabled;
}

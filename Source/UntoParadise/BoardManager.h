// Created by Kairo Forge

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BoardManager.generated.h"


class ABoardPiece;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UNTOPARADISE_API UBoardManager : public UActorComponent
{
	GENERATED_BODY()

public:
	
	UBoardManager();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Settings")
	TMap<FIntPoint, AActor*> GridPairing;
	
	UPROPERTY()
	TArray<AActor*> CachedReachablePieces;

	UPROPERTY()
	AActor* CurrentStartingPiece = nullptr;

public:	
	virtual void BeginPlay() override;
	
	UFUNCTION(BlueprintCallable)
	void AddGridPairing(FIntPoint GridCoordinates, AActor* BoardPiece);

	UFUNCTION(BlueprintCallable)
	AActor* GetGridPiece(FIntPoint GridCoordinates) const;
	
	TArray<FIntPoint> GetAllGridCoordinates() const;
	TArray<AActor*> GetAllReachablePieces() const;
	
	void HighlightAllReachablePositions(bool ActivateHighlight);
	void ResetHighlightedPositions();
	
	void ActivateDamageHighlight(TArray<FIntPoint> CoordinatesToPrime) const;

	void ClearActiveBoard();
	
	AActor* GetCurrentStartingPiece() const
	{
		return CurrentStartingPiece;
	}

	void SetCurrentStartingPiece(AActor* NewCurrentStartingPiece)
	{
		CurrentStartingPiece = NewCurrentStartingPiece;
	}

	bool DoesGridCoordinatesExist(FIntPoint Coords) const;
	bool IsGridPieceActive(FIntPoint GridCoordinates) const;
	
};

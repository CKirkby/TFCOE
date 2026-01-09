// Created by Snow Paw Games

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BoardManager.generated.h"


class ABoardPiece;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TFCOE_API UBoardManager : public UActorComponent
{
	GENERATED_BODY()

public:
	
	UBoardManager();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Settings")
	TMap<FIntPoint, AActor*> GridPairing;

	UPROPERTY()
	AActor* CurrentStartingPiece = nullptr;
	
	virtual void BeginPlay() override;

public:	
	
	UFUNCTION(BlueprintCallable)
	void AddGridPairing(FIntPoint GridCoordinates, AActor* BoardPiece);

	UFUNCTION(BlueprintCallable)
	AActor* GetGridPiece(FIntPoint GridCoordinates) const;

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
	
};

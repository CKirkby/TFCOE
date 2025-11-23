// Created by Snow Paw Games

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BoardPiece.generated.h"

class UBoxComponent;

UENUM(BlueprintType)
enum EPieceState
{
	Enabled UMETA(DisplayName = "Enabled"),
	Disabled UMETA(DisplayName = "Disabled"),
	Occupied UMETA(DisplayName = "Occupied"),
};

UCLASS()
class TFCOE_API ABoardPiece : public AActor
{
	GENERATED_BODY()
	
public:	
	ABoardPiece();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	FVector2D GridPosition;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	TEnumAsByte<EPieceState> CurrentPieceState = Enabled;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	AActor* CurrentOccupier = nullptr;
	
	virtual void BeginPlay() override;

public:
	
	// Getter and Setter

	UFUNCTION(BlueprintCallable, Category = "Board Piece")
	void SetPieceState(const EPieceState NewPieceState)
	{
		CurrentPieceState = NewPieceState;
	}

	UFUNCTION(BlueprintCallable, Category = "Board Piece")
	EPieceState GetPieceState() const
	{
		return CurrentPieceState;
	}

	UFUNCTION(BlueprintCallable, Category = "Board Piece")
	AActor* GetCurrentOccupier() const
	{
		return CurrentOccupier;
	}

	UFUNCTION(BlueprintCallable, Category = "Board Piece")
	void SetCurrentOccupier(AActor* NewOccupier)
	{
		if (NewOccupier)
		{
			CurrentOccupier = NewOccupier;
		}
	}

	UFUNCTION(BlueprintCallable, Category = "Board Piece")
	void ClearCurrentOccupier()
	{
		CurrentOccupier = nullptr;
	}

	UFUNCTION(BlueprintCallable, Category = "Board Piece")
	FVector2D GetGridPosition() const
	{
		return GridPosition;
	}
};

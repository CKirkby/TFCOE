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

UENUM(BlueprintType, meta = (Bitflags, BitmaskEnum="ETraceDirections", UseEnumValuesAsMaskValuesInEditor="true"))
enum class ETraceDirections : uint8
{
	None = 0 UMETA(Hidden),
	Up = 1 << 0,
	Down = 1 << 1,
	Left = 1 << 2,
	Right = 1 << 3,
	UpperLeft = 1 << 4,
	UpperRight = 1 << 5,
	LowerLeft = 1 << 6,
	LowerRight = 1 << 7
};
ENUM_CLASS_FLAGS(ETraceDirections)

struct FDirectionBinding
{
	ETraceDirections Direction;
	TFunction<void()> BindAction;
};

UCLASS()
class TFCOE_API ABoardPiece : public AActor
{
	GENERATED_BODY()
	
public:	
	ABoardPiece();

protected:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	TEnumAsByte<EPieceState> CurrentPieceState = Enabled;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	AActor* CurrentOccupier = nullptr;
	
	virtual void BeginPlay() override;

public:

	UFUNCTION(BlueprintCallable, Category = "Board Piece")
	TArray<FHitResult> ActivateCombatTrace(UPARAM(meta = (Bitmask, BitmaskEnum = "ETraceDirections")) int Directions);


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

	

};

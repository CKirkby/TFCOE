// Created by Snow Paw Games

#pragma once

#include "CoreMinimal.h"
#include "BoardControllerInterface.generated.h"

enum class EHighlightType
{
	None UMETA(DisplayName = "None"),
	Movement UMETA(DisplayName = "Movement"),
	Attack UMETA(DisplayName = "Attack")
};

/**
 * 
 */
UINTERFACE(Blueprintable)
class TFCOE_API UBoardControllerInterface : public UInterface
{
	GENERATED_BODY()	
};

class TFCOE_API IBoardControllerInterface
{
	GENERATED_BODY()
	
public:
	
	virtual void SetReachableMovementPositionsVisible(bool Active) = 0;
	virtual void SetAttackPositionsVisible(TArray<FIntPoint> Positions) = 0;
	
	// Tells to highlight the board piece in a specific fashion, red for combat, green for movement etc...
	virtual void NotifyHighlightBoardPiece(EHighlightType Type) = 0;
	virtual void ResetHighlightedPieces() = 0;
	
	virtual TArray<FIntPoint> GetAllBoardPieces() = 0;
	
};

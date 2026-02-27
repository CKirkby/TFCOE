// Created by Snow Paw Games

#pragma once

#include "CoreMinimal.h"
#include "BoardControllerInterface.generated.h"

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
	
	virtual void SetReachableMovementPositionsVisible(TArray<FIntPoint> Positions) = 0;
	virtual void SetAttackPositionsVisible(TArray<FIntPoint> Positions) = 0;
	
};

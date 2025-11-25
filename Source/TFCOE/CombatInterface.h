// Created by Snow Paw Games

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CombatInterface.generated.h"

enum EPieceState : int;
enum ETurnOrder : int;

UINTERFACE(Blueprintable)
class TFCOE_API UCombatInterface : public UInterface
{
	GENERATED_BODY()
};

class TFCOE_API ICombatInterface
{
	GENERATED_BODY()

	public:

	// Player
	virtual void NotifyEndTurn() = 0;
	virtual void NotifyCombatStatus(int CombatState) = 0;
	virtual FVector GetCombatPlayerLocation() = 0;
	virtual AActor* GetPlayerCombatant() = 0;
	virtual void NotifyMovementRequirementsMet(AActor* BoardPiece) = 0;
	virtual void SetCombatantCoordinates(FVector2D Coordinates) = 0;

	// Gamemode
	virtual FVector2D GetGridCoordinates() = 0;
	virtual ETurnOrder GetCurrentTurnOrder() = 0;
	virtual void NotifyEndTurnTriggered() = 0;

	// Board Piece
	virtual void NotifyPieceClicked() = 0;
	virtual FVector GetBoardPieceLocation() = 0;
	virtual EPieceState GetCurrentPieceState() = 0;
};
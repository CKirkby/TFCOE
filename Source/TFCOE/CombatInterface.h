// Created by Snow Paw Games

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "FactionID.h"
#include "EnemyTier.h"
#include "EntityID.h"
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
	virtual void NotifyEndIndividualTurn() = 0;
	virtual void NotifyCombatStatus(int CombatState) = 0;
	virtual FVector GetCombatPlayerLocation() = 0;
	virtual AActor* GetPlayerCombatant() = 0;
	virtual void NotifyMovementRequirementsMet(AActor* BoardPiece) = 0;
	virtual void SetCombatantCoordinates(FIntPoint Coordinates) = 0;

	// Gamemode
	virtual FIntPoint GetGridCoordinates() = 0;
	virtual ETurnOrder GetCurrentTurnOrder() = 0;
	virtual void NotifyEndTurnTriggered() = 0;
	virtual TArray<AActor*> GetActiveCombatantRoster() = 0;
	virtual AActor* GetGridPieceFromCoordinates(FIntPoint Coordinates) = 0;
	virtual EFactionID GetActorFactionID() = 0;
	virtual EEnemyTier GetActorFactionRank() = 0;
	virtual EEntityID GetActorEntityID() = 0;
	
	// Board Piece
	virtual void NotifyPieceClicked() = 0;
	virtual FVector GetBoardPieceLocation() = 0;
	virtual EPieceState GetCurrentPieceState() = 0;
	virtual bool DoesGridContainCoordinate(FIntPoint CoordsToCheck) = 0;

	// General
	virtual void MoveAI_Character(FVector Location) = 0;
	virtual void BeginTurnPhase() = 0;
};
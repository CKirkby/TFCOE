// Created by Snow Paw Games

#pragma once

#include "CombatInterface.h"

#include "CoreMinimal.h"
#include "CharacterCombatData.h"
#include "CombatManager.h"
#include "PaperZDCharacter.h"
#include "AI_PlayerCombatant.generated.h"

class UCharacterCombatData;

UCLASS()
class TFCOE_API AAI_PlayerCombatant : public APaperZDCharacter, public ICombatInterface
{
	GENERATED_BODY()

public:
	AAI_PlayerCombatant();

protected:

	// Components
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UCharacterCombatData* CombatData = nullptr;
	
	virtual void BeginPlay() override;

public:
	
	virtual void Tick(float DeltaTime) override;

	// Interface Implementation
	virtual void NotifyMovementRequirementsMet(AActor* BoardPiece) override;
	virtual void SetCombatantCoordinates(FVector2D Coordinates) override;
	virtual FVector2D GetGridCoordinates() override {return CombatData->GetCurrentGridCoordinates();}

	// Unneeded Interface Implementations
	// Player
	virtual void NotifyEndTurn() override {}
	virtual FVector GetCombatPlayerLocation() override {return FVector::ZeroVector;}
	virtual AActor* GetPlayerCombatant() override {return nullptr;}
	virtual void NotifyCombatStatus(int CombatState) override {}

	// Gamemode
	virtual void NotifyEndTurnTriggered() override {}
	virtual ETurnOrder GetCurrentTurnOrder() override {return ETurnOrder();}

	// Board Piece
	virtual void NotifyPieceClicked() override {}
	virtual FVector GetBoardPieceLocation() override {return FVector::ZeroVector;}
	virtual EPieceState GetCurrentPieceState() override {return EPieceState();}

};

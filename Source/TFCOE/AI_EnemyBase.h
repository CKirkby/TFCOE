// Created by Snow Paw Games

#pragma once

#include "CoreMinimal.h"
#include "CharacterCombatData.h"
#include "CombatInterface.h"
#include "PaperZDCharacter.h"
#include "AI_EnemyBase.generated.h"

class UCharacterCombatData;
/**
 * 
 */
UCLASS()
class TFCOE_API AAI_EnemyBase : public APaperZDCharacter, public ICombatInterface
{
	GENERATED_BODY()

public:
	AAI_EnemyBase();

protected:
	
	// Components
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UCharacterCombatData* CombatData = nullptr;

	// Interface Implementation
	virtual FVector2D GetGridCoordinates() override {return CombatData->GetCurrentGridCoordinates();}
	virtual void SetCombatantCoordinates(FVector2D Coordinates) override {CombatData->SetCurrentGridCoordinates(Coordinates);}

	// Unneeded Interface Implementations
	virtual void NotifyEndTurn() override {}
	virtual FVector GetCombatPlayerLocation() override {return FVector::ZeroVector;}
	virtual AActor* GetPlayerCombatant() override {return nullptr;}
	virtual void NotifyCombatStatus(int CombatState) override {}
	virtual void NotifyEndTurnTriggered() override {}
	virtual ETurnOrder GetCurrentTurnOrder() override {return ETurnOrder();}
	virtual void NotifyPieceClicked() override {}
	virtual FVector GetBoardPieceLocation() override {return FVector::ZeroVector;}
	virtual EPieceState GetCurrentPieceState() override {return EPieceState();}
	virtual void NotifyMovementRequirementsMet(AActor* BoardPiece) override {}
	
	
};

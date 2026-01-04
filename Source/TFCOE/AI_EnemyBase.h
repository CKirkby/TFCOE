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

private:
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
	virtual void MoveAI_Character(FVector Location) override;
	virtual void BeginTurnPhase() override;
	virtual EFactionID GetActorFactionID() override;
	virtual EEnemyTier GetActorFactionRank() override;

	// Unneeded Interface Implementations
	virtual void NotifyEndIndividualTurn() override {}
	virtual FVector GetCombatPlayerLocation() override {return FVector::ZeroVector;}
	virtual AActor* GetPlayerCombatant() override {return nullptr;}
	virtual void NotifyCombatStatus(int CombatState) override {}
	virtual void NotifyEndTurnTriggered() override {}
	virtual ETurnOrder GetCurrentTurnOrder() override {return ETurnOrder();}
	virtual void NotifyPieceClicked() override {}
	virtual FVector GetBoardPieceLocation() override {return FVector::ZeroVector;}
	virtual EPieceState GetCurrentPieceState() override {return EPieceState();}
	virtual void NotifyMovementRequirementsMet(AActor* BoardPiece) override {}
	virtual AActor* GetGridPieceFromCoordinates(FVector2D Coordinates) override {return nullptr;}
	virtual TArray<AActor*> GetActiveCombatantRoster() override {return TArray<AActor*>();}
	virtual EEntityID GetActorEntityID() override {return EEntityID::Uninitialized_ID;}
	
	
};

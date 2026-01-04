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
private:
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
	virtual void MoveAI_Character(FVector Location) override;

	// Unneeded Interface Implementations
	// Player
	virtual void NotifyEndIndividualTurn() override {}
	virtual FVector GetCombatPlayerLocation() override {return FVector::ZeroVector;}
	virtual AActor* GetPlayerCombatant() override {return nullptr;}
	virtual void NotifyCombatStatus(int CombatState) override {}

	// Gamemode
	virtual void NotifyEndTurnTriggered() override {}
	virtual ETurnOrder GetCurrentTurnOrder() override {return ETurnOrder();}
	virtual AActor* GetGridPieceFromCoordinates(FVector2D Coordinates) override {return nullptr;}
	virtual void BeginTurnPhase() override {}
	virtual TArray<AActor*> GetActiveCombatantRoster() override {return TArray<AActor*>();}
	virtual EFactionID GetActorFactionID() override {return EFactionID::None;}
	virtual EEnemyTier GetActorFactionRank() override {return EEnemyTier::Grunt;}
	virtual EEntityID GetActorEntityID() override {return EEntityID::Uninitialized_ID;}

	// Board Piece
	virtual void NotifyPieceClicked() override {}
	virtual FVector GetBoardPieceLocation() override {return FVector::ZeroVector;}
	virtual EPieceState GetCurrentPieceState() override {return EPieceState();}

};

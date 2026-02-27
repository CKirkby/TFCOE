// Created by Snow Paw Games

#pragma once

#include "CombatInterface.h"

#include "CoreMinimal.h"
#include "CharacterCombatData.h"
#include "CombatManager.h"
#include "HealthComponent.h"
#include "HealthInterface.h"
#include "PaperZDCharacter.h"
#include "AI_PlayerCombatant.generated.h"

class UHealthComponent;
class UCharacterCombatData;

UCLASS()
class TFCOE_API AAI_PlayerCombatant : public APaperZDCharacter, public ICombatInterface, public IHealthInterface
{

private:
	GENERATED_BODY()

public:
	AAI_PlayerCombatant();

protected:

	// Components
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UCharacterCombatData* CombatData = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UHealthComponent* HealthComp = nullptr;
	
	virtual void BeginPlay() override;
	void InitialiseHealth() const;

public:
	
	virtual void Tick(float DeltaTime) override;

	// Interface Implementation
	virtual void NotifyMovementRequirementsMet(AActor* BoardPiece) override;
	virtual void SetCombatantCoordinates(FIntPoint Coordinates) override;
	virtual FIntPoint GetGridCoordinates() override {return CombatData->GetCurrentGridCoordinates();}
	virtual void MoveAI_Character(FVector Location) override;
	virtual void BeginTurnPhase() override;
	virtual void SetAttackerReference(AActor* AttackerReference) override;
	virtual int GetTimePoints() override {return CombatData->GetTimePoints();}

	// Health Interface
	virtual void SetHealth(int NewHealth) override {HealthComp->SetHealth(NewHealth);}
	virtual void TakeDamage(int IncomingDamage) override {HealthComp->TakeDamage(IncomingDamage);}
	virtual void AddHealth(int IncomingHealth) override {HealthComp->AddHealth(IncomingHealth);}
	virtual int GetHealth() override {return HealthComp->GetHealth();}
	virtual bool IsDead() override {return HealthComp->IsDead();}

	// Unneeded Interface Implementations
	// Player
	virtual void NotifyEndIndividualTurn() override {}
	virtual FVector GetCombatPlayerLocation() override {return FVector::ZeroVector;}
	virtual AActor* GetPlayerCombatant() override {return nullptr;}
	virtual void NotifyCombatStatus(int CombatState) override {}

	// Gamemode
	virtual void NotifyEndTurnTriggered() override {}
	virtual ETurnOrder GetCurrentTurnOrder() override {return ETurnOrder();}
	virtual AActor* GetGridPieceFromCoordinates(FIntPoint Coordinates) override {return nullptr;}
	virtual TArray<AActor*> GetActiveCombatantRoster() override {return TArray<AActor*>();}
	virtual EFactionID GetActorFactionID() override {return EFactionID::None;}
	virtual EEnemyTier GetActorFactionRank() override {return EEnemyTier::Grunt;}
	virtual EEntityID GetActorEntityID() override {return EEntityID::Uninitialized_ID;}

	// Board Piece
	virtual void NotifyPieceClicked() override {}
	virtual FVector GetBoardPieceLocation() override {return FVector::ZeroVector;}
	virtual EPieceState GetCurrentPieceState() override {return EPieceState();}
	virtual bool DoesGridContainCoordinate(FIntPoint CoordsToCheck) override {return false;}

};

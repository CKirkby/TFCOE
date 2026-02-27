// Created by Snow Paw Games

#pragma once

#include "CoreMinimal.h"
#include "CharacterCombatData.h"
#include "CombatInterface.h"
#include "HealthComponent.h"
#include "HealthInterface.h"
#include "PaperZDCharacter.h"
#include "AI_EnemyBase.generated.h"

class UHealthComponent;
class UCharacterCombatData;
/**
 * 
 */
UCLASS()
class TFCOE_API AAI_EnemyBase : public APaperZDCharacter, public ICombatInterface, public IHealthInterface
{

private:
	GENERATED_BODY()

public:
	AAI_EnemyBase();

	void InitialiseHealth() const;

protected:

	virtual void BeginPlay() override;
	
	// Components
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UCharacterCombatData* CombatData = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UHealthComponent* HealthComp = nullptr;

	// Interface Implementation
	virtual FIntPoint GetGridCoordinates() override {return CombatData->GetCurrentGridCoordinates();}
	virtual void SetCombatantCoordinates(FIntPoint Coordinates) override {CombatData->SetCurrentGridCoordinates(Coordinates);}
	virtual void MoveAI_Character(FVector Location) override;
	virtual void BeginTurnPhase() override;
	virtual EFactionID GetActorFactionID() override;
	virtual EEnemyTier GetActorFactionRank() override;
	virtual void SetAttackerReference(AActor* AttackerReference) override;

	// Health Interface function
	virtual void SetHealth(int NewHealth) override {HealthComp->SetHealth(NewHealth);}
	virtual void TakeDamage(int IncomingDamage) override {HealthComp->TakeDamage(IncomingDamage);}
	virtual void AddHealth(int IncomingHealth) override {HealthComp->AddHealth(IncomingHealth);}
	virtual int GetHealth() override {return HealthComp->GetHealth();}
	virtual bool IsDead() override {return HealthComp->IsDead();}

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
	virtual AActor* GetGridPieceFromCoordinates(FIntPoint Coordinates) override {return nullptr;}
	virtual TArray<AActor*> GetActiveCombatantRoster() override {return TArray<AActor*>();}
	virtual EEntityID GetActorEntityID() override {return EEntityID::Uninitialized_ID;}
	virtual bool DoesGridContainCoordinate(FIntPoint CoordsToCheck) override {return false;}
	virtual int GetTimePoints() override {return 0;}
	
};

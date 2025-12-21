// Created by Snow Paw Games

#pragma once

#include "CombatInterface.h"
#include "EncounterInterface.h"

#include "CoreMinimal.h"
#include "CombatManager.h"
#include "GameFramework/GameModeBase.h"
#include "MainGameMode.generated.h"

class UBoardManager;
class UCombatManager;
/**
 * 
 */
UCLASS()
class TFCOE_API AMainGameMode : public AGameModeBase, public ICombatInterface, public IEncounterInterface
{

private:
	GENERATED_BODY()

public:
	AMainGameMode();

protected:
	
	// Components
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UCombatManager* CombatManager = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UBoardManager* BoardManager = nullptr;

	


public:
	virtual void BeginPlay() override;
	virtual void Tick( float DeltaTime ) override;

	// Functions
	UFUNCTION(BlueprintCallable, Category = "Gamemode")
	void InitialiseCombatState(int CombatState);

	void MovePlayerToStartingPos();
	void MoveEnemiesToStartingPos(TMap<AActor*, ABoardPiece*> NewCombatants);
	
	// Interface Implementations

	// Encounter Interface
	virtual void BeginCombat() override;
	virtual void InitialiseActiveBoard(TArray<AActor*> ActivePieces, AActor* StartingPiece) override;
	virtual void InitialiseActiveCombatants(TMap<AActor*, ABoardPiece*> ActiveCombatants) override;

	// Combat Interface 
	virtual void NotifyEndTurnTriggered() override;
	virtual ETurnOrder GetCurrentTurnOrder() override;
	virtual AActor* GetGridPieceFromCoordinates(FVector2D Coordinates) override;
	virtual TArray<AActor*> GetActiveCombatantRoster() override;

	// Unneeded Interface Implementations
	// Player
	virtual void NotifyEndTurn() override {}
	virtual FVector GetCombatPlayerLocation() override {return FVector::ZeroVector;}
	virtual AActor* GetPlayerCombatant() override {return nullptr;}
	virtual void NotifyMovementRequirementsMet(AActor* BoardPiece) override {}
	virtual void NotifyCombatStatus(int CombatState) override {}
	virtual void SetCombatantCoordinates(FVector2D Coordinates) override {}

	// Gamemode
	virtual FVector2D GetGridCoordinates() override {return FVector2D::ZeroVector;}
	virtual void MoveAI_Character(FVector Location) override {}
	virtual void BeginTurnPhase() override {}
	virtual EFactionID GetActorFactionID() override {return EFactionID::None;}
	virtual EEnemyTier GetActorFactionRank() override {return EEnemyTier::Grunt;}

	// Board Piece
	virtual void NotifyPieceClicked() override {}
	virtual FVector GetBoardPieceLocation() override {return FVector::ZeroVector;}
	virtual EPieceState GetCurrentPieceState() override {return EPieceState();}
	
};

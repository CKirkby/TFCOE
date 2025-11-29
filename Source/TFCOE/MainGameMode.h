// Created by Snow Paw Games

#pragma once

#include "CombatInterface.h"

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
class TFCOE_API AMainGameMode : public AGameModeBase, public ICombatInterface
{
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
	void InitialiseCombatState(int CombatState) const;
	
	// Interface Implementations
	virtual void NotifyEndTurnTriggered() override;
	virtual ETurnOrder GetCurrentTurnOrder() override;

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

	// Board Piece
	virtual void NotifyPieceClicked() override {}
	virtual FVector GetBoardPieceLocation() override {return FVector::ZeroVector;}
	virtual EPieceState GetCurrentPieceState() override {return EPieceState();}
	
};

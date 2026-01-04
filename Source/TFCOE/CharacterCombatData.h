// Created by Snow Paw Games

#pragma once

#include "CoreMinimal.h"
#include "EnemyBehaviour.h"
#include "Components/ActorComponent.h"
#include "CharacterCombatData.generated.h"

class ICombatInterface;
class UEnemyBehaviour;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TFCOE_API UCharacterCombatData : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCharacterCombatData();

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	UEnemyBehaviour* EntityCombatConfiguration;

	// The target the AI Will pursue. Not necessary for player
	UPROPERTY()
	AActor* CurrentTarget = nullptr;
	UPROPERTY()
	AActor* PreviousAttacker = nullptr;
	bool AttackedLastTurn = false;
	
	// Action Points
	int TimePoints = 10;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	int MaxTimePoints = 10;
	
	// Movement
	FVector2D CurrentGridCoordinates = FVector2D::ZeroVector;

	// Interfaces
	ICombatInterface* CombatInterfaceGamemode = nullptr;
	
	virtual void BeginPlay() override;

public:

	// Activates this characters turn. 
	void ExecuteCurrentTurn();
	void EndThisActorTurn() const;

	// If AI, it will select a character to target. Most of the time it will be the player. 
	AActor* SelectTargetForTurn();

	// Checks if the character should actually move, is it already next to target etc...
	bool CheckShouldMove(FVector2D PlayerCoordinates);

	// Used to check if the actor has enough action points to move to that spot.
	bool CheckCanAffordMovement(FVector2D CurrentCoordinates, FVector2D TargetCoordinates);

	// The math part to the above function.
	int CalculateMovementCost(FVector2D CurrentCoordinates, const FVector2D TargetCoordinates);
	
	FVector2D CalculateTargetMovementPiece() const;
	EFactionID GetFactionID() const;
	EEnemyTier GetFactionRank() const;
	TArray<AActor*> SortCombatantsByDistance(const TArray<AActor*>& Combatants) const;
	TArray<AActor*> GetCombatantsByFaction(TArray<AActor*> CombatantsToCheck, EFactionID FactionToCheck);
	AActor* GetTargetFromClosestOrRandom(TArray<AActor*> PotentialTargets, float Weight) const;
	
	// Getter and Setter //
	
	UFUNCTION(BlueprintCallable, Category = "CombatData")
	FVector2D GetCurrentGridCoordinates() const
	{
		return CurrentGridCoordinates;
	}

	UFUNCTION(BlueprintCallable, Category = "CombatData")
	void SetCurrentGridCoordinates(const FVector2D Coordinates)
	{
		CurrentGridCoordinates = Coordinates;
	}

	UFUNCTION(BlueprintCallable, Category = "CombatData")
	int GetTimePoints() const
	{
		return TimePoints;
	}

	UFUNCTION(BlueprintCallable, Category = "CombatData")
	void SetTimePoints(const int NewTimePoints)
	{
		TimePoints = NewTimePoints;
	}

	UFUNCTION(BlueprintCallable, Category = "CombatData")
	void ResetTimePoints()
	{ 
		TimePoints = MaxTimePoints;
	}
};

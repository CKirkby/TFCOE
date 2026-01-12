// Created by Snow Paw Games

#pragma once

#include "CoreMinimal.h"
#include "EnemyBehaviour.h"
#include "Components/ActorComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "CharacterCombatData.generated.h"

struct FAIRequestID;
class ICombatInterface;
class UEnemyBehaviour;

struct FAStarNode
{
	FIntPoint Coordinates;
	int32 TotalCost = 0;
};

static bool AStarHeapLess(const FAStarNode& PointA, const FAStarNode& PointB)
{
	return PointA.TotalCost > PointB.TotalCost;
}

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
	int32 TimePoints = 10;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	int32 MaxTimePoints = 10;
	
	// Movement
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Settings")
	FIntPoint CurrentGridCoordinates = FIntPoint::ZeroValue;
	TArray<FIntPoint> TurnPath;
	int32 TurnPathIndex = 0;

	// Interfaces
	ICombatInterface* CombatInterfaceGamemode = nullptr;
	ICombatInterface* CombatInterfacePlayer = nullptr;

	
	virtual void BeginPlay() override;

public:

	// Activates this characters turn. 
	void ExecuteCurrentTurn();
	void EndThisActorTurn() const;

	// If AI, it will select a character to target. Most of the time it will be the player. 
	AActor* SelectTargetForTurn();

	// Checks if the character should actually move, is it already next to target etc...
	bool CheckShouldMove(const FAttackConfiguration* ChosenAttack, AActor* ChosenTarget);
	bool CheckIsAdjacent(FIntPoint& PointA, FIntPoint& PointB) const;
	int32 GetGridDistanceAllDir(const FIntPoint& PointA, const FIntPoint& PointB) const;
	int32 GetGridDistanceCardinal(const FIntPoint& PointA, const FIntPoint& PointB) const;
	bool IsAlignedCardinal(FIntPoint& PointA, FIntPoint& PointB) const;
	bool IsAlignedAllDir(FIntPoint& PointA, FIntPoint& PointB) const;

	// Stores a chosen attack.
	FAttackConfiguration* ChooseAttackForTurn(AActor* TargetActor) const;
	FAttackConfiguration* GetAttackFromType(EAttackType AttackType) const;

	// AI movement functions
	FIntPoint ChooseMovementPosition(AActor* TargetActor);
	bool CalculatePathToPosition(FIntPoint& Start, const FIntPoint& Target, int& OutSteps) const;
	bool FindPathUsingAStar(FIntPoint& StartCoords, const FIntPoint& TargetCoords, TArray<FIntPoint>& OutPath) const;

	UFUNCTION()
	void OnMovementComplete(FAIRequestID RequestID, EPathFollowingResult::Type Result);
	void StartMovementAlongGridPath(const TArray<FIntPoint>& Path);
	void MoveToNextGridPos();
	FVector GetGridPosition(const FIntPoint& Coordinates) const;

	// Pathfinding
    void GetGridAdjacentAllDir(const FIntPoint& OriginCoordinates, TArray<FIntPoint>& OutNeighbors) const;	
	
	// Used to check if the actor has enough action points to move to that spot.
	bool CheckCanAffordMovement(FIntPoint CurrentCoordinates, FIntPoint TargetCoordinates);

	// The math part to the above function.
	int32 CalculateMovementCost(FIntPoint CurrentCoordinates, FIntPoint TargetCoordinates);

	FIntPoint CalculateTargetMovementPiece() const;
	EFactionID GetFactionID() const;
	EEnemyTier GetFactionRank() const;
	TArray<AActor*> SortCombatantsByDistance(const TArray<AActor*>& Combatants) const;
	TArray<AActor*> GetCombatantsByFaction(TArray<AActor*> CombatantsToCheck, EFactionID FactionToCheck);
	AActor* GetTargetFromClosestOrRandom(TArray<AActor*> PotentialTargets, float Weight) const;

	void DelayLambda(float DelayTime, TFunction<void()> Function);

	// Grid Piece checker
	bool DoesGridCoordinatesExist(const FIntPoint GridCoordinates) const;
	bool IsGridPieceActive(const FIntPoint GridCoordinates) const;
	
	// Getter and Setter //
	
	UFUNCTION(BlueprintCallable, Category = "CombatData")
	FIntPoint GetCurrentGridCoordinates() const
	{
		return CurrentGridCoordinates;
	}

	UFUNCTION(BlueprintCallable, Category = "CombatData")
	void SetCurrentGridCoordinates(const FIntPoint Coordinates)
	{
		CurrentGridCoordinates = Coordinates;
	}

	UFUNCTION(BlueprintCallable, Category = "CombatData")
	int32 GetTimePoints() const
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

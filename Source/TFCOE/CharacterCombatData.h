// Created by Snow Paw Games

#pragma once

#include "CoreMinimal.h"
#include "UnitConfiguration.h"
#include "Components/ActorComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "CharacterCombatData.generated.h"

struct FAIRequestID;
class ICombatInterface;
class UUnitConfiguration;

struct FCandidatePathway
{
	FIntPoint GridPoint;
	int32 Distance = 0;
};

struct FAStarGrid
{
	FIntPoint GridPoint;
	int32 Cost = 0; // How many steps it will take. 
	int32 Heuristic = 0; // Distance it will be
	int32 Final() const {return Cost + Heuristic;}
};

static bool AStarHeapCheck(const FAStarGrid& PointA, const FAStarGrid& PointB)
{
	// We want the lowest possible moves and distance so this organises them by that standard
	const int32 AFinal = PointA.Final();
	const int32 BFinal = PointB.Final();
	if (AFinal != BFinal) return AFinal < BFinal;

	return PointA.Heuristic < PointB.Heuristic;
}

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TFCOE_API UCharacterCombatData : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCharacterCombatData();
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAttackCommence, EAttackType, AttackType, bool, HitSuccessful);
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnAttackCommence OnAttackCommence;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	UUnitConfiguration* UnitCombatConfiguration;

	// The target the AI Will pursue. Not necessary for player
	UPROPERTY()
	AActor* CurrentTarget = nullptr;
	UPROPERTY()
	AActor* PreviousAttacker = nullptr;
	FAttackConfiguration* CurrentAttack = nullptr;
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
	
	// Turn Phase Settings
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Turn_Phase")
	float SelectTargetDelay = 0.25f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Turn_Phase")
	float SelectAttackDelay = 0.1f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Turn_Phase")
	float MovementDelay = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Turn_Phase")
	float AttackDelay = 0.5f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Turn_Phase")
	float EndTurnDelay = 0.5f;

	// Interfaces
	ICombatInterface* CombatInterfaceGamemode = nullptr;
	ICombatInterface* CombatInterfacePlayer = nullptr;

	
	virtual void BeginPlay() override;

public:

	// Activates this characters turn. 
	void ExecuteCurrentTurn();
	void StepOne_SelectTarget();
	void StepTwo_SelectAttack();
	void StepThree_Movement();
	void EndActorTurn();

	// If AI, it will select a character to target. Most of the time it will be the player. 
	AActor* SelectTargetForTurn();
	bool UpdatePlayerTurn();

	// Checks if the character should actually move, is it already next to target etc...
	bool CheckShouldMove(const FAttackConfiguration* ChosenAttack, AActor* ChosenTarget);
	bool CheckIsAdjacent(FIntPoint& PointA, FIntPoint& PointB) const;
	int32 GetGridDistanceAllDir(const FIntPoint& PointA, const FIntPoint& PointB) const;
	int32 GetGridDistanceCardinal(const FIntPoint& PointA, const FIntPoint& PointB) const;
	int32 GetGridDistanceOrdinal(const FIntPoint& PointA, const FIntPoint& PointB) const;
	bool IsAlignedCardinal(const FIntPoint& PointA, const FIntPoint& PointB) const;
	bool IsAlignedAllDir(const FIntPoint& PointA, const FIntPoint& PointB) const;
	bool IsAlignedOrdinal(const FIntPoint& PointA, const FIntPoint& PointB) const;

	// Stores a chosen attack.
	FAttackConfiguration* ChooseAttackForTurn(AActor* TargetActor);
	FAttackConfiguration* GetAttackFromType(EAttackType AttackType) const;

	// AI movement functions
	TArray<FCandidatePathway> GetReachableMovementPositions(AActor* TargetActor, FAttackConfiguration* ChosenAttack);
	TArray<FIntPoint> ChooseValidMovementPath(const TArray<FCandidatePathway>& PossiblePositions, int32 PathwayAttemptModifier);
	bool FindPathUsingAStar(FIntPoint& StartCoords, const FIntPoint& TargetCoords, TArray<FIntPoint>& OutPath) const;

	UFUNCTION()
	void OnMovementComplete(FAIRequestID RequestID, EPathFollowingResult::Type Result);
	void StartMovementAlongGridPath(const TArray<FIntPoint>& Path);
	void MoveToNextGridPos();
	FVector GetGridPosition(const FIntPoint& Coordinates) const;
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

	FIntPoint GetCurrentTargetCoords(AActor* Target);
	
	// Combat Functionality
	bool CanAttackFromPosition(FAttackConfiguration* ChosenAttack, const FIntPoint& PointA, const FIntPoint& PointB);
	void PerformAttack(const FAttackConfiguration* ChosenAttack);
	void DelayLambda(float DelayTime, TFunction<void()> Function);
	void SetAttackerReference();

	// Grid Piece checker
	bool DoesGridCoordinatesExist(const FIntPoint GridCoordinates) const;
	bool IsGridPieceActive(const FIntPoint GridCoordinates) const;
	
	void CheckToResetMovementHighlights() const;
	
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
	
	void RemoveTimePoints(const int32 Amount);

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

	int GetHealthConfig() const
	{
		if (UnitCombatConfiguration)
		{
			return UnitCombatConfiguration->CombatConfiguration.Health;
		}
		
		return 1;
	}
	
	UFUNCTION(BlueprintCallable, Category = "CombatData")
	UUnitConfiguration* GetUnitConfiguration() const
	{
		return UnitCombatConfiguration;
	}
	
	void SetPreviousAttacker(AActor* AttackerReference)
	{
		PreviousAttacker = AttackerReference;
	}
	
	void ClearPreviousAttacker()
	{
		PreviousAttacker = nullptr;
	}
};

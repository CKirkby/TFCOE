// Created by Snow Paw Games

#pragma once

#include "CombatInterface.h"

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatManager.generated.h"

class ABoardPiece;

UENUM(BlueprintType)
enum ETurnOrder
{
	Player UMETA(DisplayName = "Player"),
	Enemy UMETA(DisplayName = "Enemy"),
	Companion UMETA(DisplayName = "Companion"),
	None UMETA(DisplayName = "None")
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TFCOE_API UCombatManager : public UActorComponent, public ICombatInterface
{
	GENERATED_BODY()

public:	
	UCombatManager();

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCombatBegin);
	UPROPERTY(BlueprintAssignable, Category="Events")
	FOnCombatBegin OnCombatBegin;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCombatEnd);
	UPROPERTY(BlueprintAssignable, Category="Events")
	FOnCombatEnd OnCombatEnd;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Settings|Combat State")
	int CurrentCombatState = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Turn Order")
	TMap<int, TEnumAsByte<ETurnOrder>> TurnOrder;

	ETurnOrder CurrentTurnOrder = None;
	int CurrentTurnIndex = 1;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	/**
	 * 0 -> Disengaged
	 * 1 -> Engaged
	 * @param CombatState The state to set the combat into. 
	 */
	UFUNCTION(BlueprintCallable, Category="CombatManager")
	void SetCombatState(int CombatState);

	void SetTurnOrder(int NewTurnOrder);
	UFUNCTION(BlueprintCallable, Category="CombatManager")
	void EndCurrentTurn();
	UFUNCTION(BlueprintCallable, Category="CombatManager")
	void EndCombat();

	/**
	 * 0 -> Disengaged
	 * 1 -> Engaged
	 */
	UFUNCTION(BlueprintCallable, Category="CombatManager")
	int GetCombatState() const
	{
		return CurrentCombatState;
	}

	UFUNCTION(BlueprintCallable, Category="CombatManager")
	ETurnOrder GetCurrentTurnOrder() const
	{
		return CurrentTurnOrder;
	}

	UFUNCTION(BlueprintCallable, Category="CombatManager")
	int GetCurrentTurnIndex() const
	{
		return CurrentTurnIndex;
	}

	// Interface Implementation
	void NotifyPlayerOfCombatStatus(int CombatState) const;
	virtual ETurnOrder GetCurrentTurnOrder() override {return CurrentTurnOrder;}

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
	virtual void NotifyEndTurnTriggered() override {}

	// Board Piece
	virtual void NotifyPieceClicked() override {}
	virtual FVector GetBoardPieceLocation() override {return FVector::ZeroVector;}
	virtual EPieceState GetCurrentPieceState() override {return EPieceState();}
};

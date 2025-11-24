// Created by Snow Paw Games

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatManager.generated.h"

UENUM(BlueprintType)
enum ETurnOrder
{
	Player UMETA(DisplayName = "Player"),
	Enemy UMETA(DisplayName = "Enemy"),
	Companion UMETA(DisplayName = "Companion"),
	None UMETA(DisplayName = "None")
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TFCOE_API UCombatManager : public UActorComponent
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
};

// Created by Snow Paw Games

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatManager.generated.h"

UENUM()
enum ETurnOrder
{
	Player UMETA(DisplayName = "Player"),
	Enemy UMETA(DisplayName = "Enemy"),
	Companion UMETA(DisplayName = "Companion")
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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Settings")
	int CurrentCombatState = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	TMap<TEnumAsByte<ETurnOrder>, int> TurnOrder;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	
	/**
	 * 0 -> Disengaged
	 * 1 -> Engaged
	 * @param CombatState The state to set the combat into. 
	 */
	UFUNCTION(BlueprintCallable, Category="Combat")
	void SetCombatState(int CombatState);

	UFUNCTION(BlueprintCallable, Category="Combat")
	int GetCombatState() const
	{
		return CurrentCombatState;
	}
};

// Created by Snow Paw Games

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatManager.generated.h"

UENUM(BlueprintType)
enum ECombatState
{
	Engaged UMETA(DisplayName = "Engaged"),
	Disengaged UMETA(DisplayName = "Disengaged")
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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	TEnumAsByte<ECombatState> CurrentCombatState = Disengaged;
	
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	UFUNCTION(BlueprintCallable, Category="Combat")
	void SetCombatState(ECombatState NewState);
};

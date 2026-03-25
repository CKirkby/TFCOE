// Created by Snow Paw Games

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttackController.generated.h"


struct FAttackConfiguration;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UNTOPARADISE_API UAttackController : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAttackController();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
	UPROPERTY()
	AActor* PlayerCombatantObjRef = nullptr;

public:
	bool PerformBasicAttack(AActor* Target, const FAttackConfiguration* ChosenAttack) const;
	void SetAttackerReference(AActor* Target) const;
	
	void SetPlayerCombatantReference(AActor* PlayerCombatant)
	{
		PlayerCombatantObjRef = PlayerCombatant;
	}
	
	void ClearPlayerCombatantReference()
	{
		PlayerCombatantObjRef = nullptr;
	}
};

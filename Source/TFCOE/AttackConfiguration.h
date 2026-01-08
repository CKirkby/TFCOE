// Created by Snow Paw Games

#pragma once

#include "CoreMinimal.h"
#include "AttackConfiguration.generated.h"

UENUM(BlueprintType)
enum class EAttackType : uint8
{
	Close UMETA(DisplayName = "Close Combat"),
	Ranged UMETA(DisplayName = "Ranged Combat")
};

USTRUCT(BlueprintType)
struct FAttackConfiguration
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack Settings")
	FName AttackID = FName("");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack Settings")
	EAttackType AttackType = EAttackType::Close;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack Settings")
	bool RequiresAlignment = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack Settings", meta = (ClampMin = "1"))
	int AttackRange = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack Settings", meta = (EditCondition = "AttackType == EAttackType::Ranged", EditConditionHides, Clamp = "1"))
	int MaxAttackRange = 1;

	//Attack Grid coords. Direction based? Tarray intpoints?
	//Damage
	//Cooldown
	
};

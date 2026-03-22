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

UENUM(BlueprintType)
enum class EAttackRangeType : uint8
{
	Cardinal UMETA(DisplayName = "Cardinal"),
	Ordinal UMETA(DisplayName = "Ordinal"),
	Both UMETA(DisplayName = "Both")
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack Settings")
	EAttackRangeType RangeType = EAttackRangeType::Both;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack Settings")
	int AttackDamage = 1;
		
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack Settings", meta = (ClampMin = "1"))
	int AttackRange = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack Settings", meta = (EditCondition = "AttackType == EAttackType::Ranged", EditConditionHides, Clamp = "1"))
	int MaxAttackRange = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack Settings")
	int AttackCooldown = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack Settings")
	float BaseHitChance = 80.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack Settings")
	int AP_Cost = 0;
	
	
	//Attack Grid coords. Direction based? Tarray intpoints?
	
};

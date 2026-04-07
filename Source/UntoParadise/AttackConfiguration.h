// Created by Kairo Forge

#pragma once

#include "CoreMinimal.h"
#include "AttackConfiguration.generated.h"

UENUM(BlueprintType)
enum class EAttackType : uint8
{
	Close UMETA(DisplayName = "Close Combat"),
	Ranged UMETA(DisplayName = "Ranged Combat"),
};

UENUM(BlueprintType)
enum class EAttackRangeType : uint8
{
	Cardinal UMETA(DisplayName = "Cardinal"),
	Ordinal UMETA(DisplayName = "Ordinal"),
	Both UMETA(DisplayName = "Both")
};

UENUM(BlueprintType)
enum class EAttackFormat : uint8
{
	Regular UMETA(DisplayName = "Regular"),
	Special UMETA(DisplayName = "Special")
};

UENUM(BlueprintType)
enum class EDirectionalFacing : uint8
{
	Any UMETA(DisplayName = "Any Direction"),
	Up UMETA(DisplayName = "Up"),
	Down UMETA(DisplayName = "Down"),
	Left UMETA(DisplayName = "Left"),
	Right UMETA(DisplayName = "Right")
};

UENUM()
enum class EAttackOriginPoint : uint8
{
	Self UMETA(DisplayName = "Self"),
	Target UMETA(DisplayName = "Target")
};

USTRUCT(BlueprintType)
struct FAttackCoordination
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EDirectionalFacing Direction = EDirectionalFacing::Any;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FIntPoint> Positions = {};
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
	EAttackFormat AttackFormat = EAttackFormat::Regular;

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
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack Settings", meta = (ClampMin = "1", ClampMax = "100"))
	float BaseHitChance = 80.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack Settings")
	int AP_Cost = 0;
	
	// Special Attack settings \\
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack Settings", meta = (EditCondition = "AttackFormat == EAttackFormat::Special", EditConditionHides))
	float ChanceToUseAvailableSpecial = 85.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack Settings", meta = (EditCondition = "AttackFormat == EAttackFormat::Special", EditConditionHides))
	int ActivationTime = 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack Settings", meta = (EditCondition = "AttackFormat == EAttackFormat::Special", EditConditionHides))
	int AttackCooldown = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack Settings", meta = (EditCondition = "AttackFormat == EAttackFormat::Special", EditConditionHides))
	EAttackOriginPoint AttackOrigin = EAttackOriginPoint::Self;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack Settings", meta = (EditCondition = "AttackFormat == EAttackFormat::Special && AttackOrigin == EAttackOriginPoint::Self", EditConditionHides))
	bool OrientToTarget = true;
	
	// The attack coordinates to be determined. 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack Settings", meta = (EditCondition = "AttackFormat == EAttackFormat::Special", EditConditionHides))
	TArray<FAttackCoordination> ImpactCoordination = {};
	
};

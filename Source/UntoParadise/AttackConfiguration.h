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

	// Whether the attack is a regular attack or a special attack. Special attacks have extended settings.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack Settings")
	EAttackFormat AttackFormat = EAttackFormat::Regular;  

	// Weather the attack needs the current character to be aligned with the target on the grid, same row or column or diagonal etc...
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack Settings")
	bool RequiresAlignment = true;

	// Determines what axis this attack will use for attacks, if its cardinal it can only attack on the left right up down, etc... and the same for ordinal but diagonals.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack Settings")
	EAttackRangeType RangeType = EAttackRangeType::Both;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack Settings")
	int AttackDamage = 1;
		
	// The amount of grid squares it needs to be before it can be considered a valid attack, if ranged attack this is the minimum.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack Settings", meta = (ClampMin = "1"))
	int AttackRange = 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack Settings", meta = (EditCondition = "AttackType == EAttackType::Ranged", EditConditionHides, Clamp = "1"))
	int MaxAttackRange = 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack Settings", meta = (ClampMin = "1", ClampMax = "100"))
	float BaseHitChance = 80.0f;

	// Player Only: This is a setting for the player, the AP cost will be taken from the players pool upon execution.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack Settings")
	int AP_Cost = 0;
	
	// Special Attack settings \\
	
	// A extra int to determine a chance to use this attack if it selected to be used. This is to add some variability to the special attacks, so they arent always used in combat.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack Settings", meta = (EditCondition = "AttackFormat == EAttackFormat::Special", EditConditionHides, ClampMin = "1", ClampMax = "100"))
	float ChanceToUseAvailableSpecial = 85.0f;
	
	// The time it takes to prime and activate the attack, if 1 it will just activate immediatly. 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack Settings", meta = (EditCondition = "AttackFormat == EAttackFormat::Special", EditConditionHides))
	int ActivationTime = 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack Settings", meta = (EditCondition = "AttackFormat == EAttackFormat::Special", EditConditionHides))
	int AttackCooldown = 0;
	
	// Determines the origin point of where the grid calculates for danger zones / AOEs will take place.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack Settings", meta = (EditCondition = "AttackFormat == EAttackFormat::Special", EditConditionHides))
	EAttackOriginPoint AttackOrigin = EAttackOriginPoint::Self;

	// Face the target when executing the attack
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack Settings", meta = (EditCondition = "AttackFormat == EAttackFormat::Special", EditConditionHides))
	bool OrientToTarget = true;
	
	// Determines whether or not it can damage the factions allies.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack Settings", meta = (EditCondition = "AttackFormat == EAttackFormat::Special", EditConditionHides))
	bool FriendlyFireEnabled = false;
	
	// The attack coordinates to be determined. 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack Settings", meta = (EditCondition = "AttackFormat == EAttackFormat::Special", EditConditionHides))
	TArray<FAttackCoordination> ImpactCoordination = {};
	
};

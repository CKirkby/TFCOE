// Created by Snow Paw Games

#pragma once

#include "CoreMinimal.h"
#include "AttackConfiguration.h"
#include "CombatConfiguration.h"
#include "EnemyTier.h"
#include "Engine/DataAsset.h"
#include "UnitConfiguration.generated.h"

struct FAttackConfiguration;
struct FCombatConfiguration;
enum class EFactionID : uint8;
enum class EEntityID : uint8;

UENUM()
enum class ECombatStyle : uint8
{
	Any UMETA(DisplayName = "Any"),
	PreferClose UMETA(DisplayName = "PreferClose"),
	PreferRanged UMETA(DisplayName = "PreferRanged")
};

/**
 * 
 */
UCLASS()
class TFCOE_API UUnitConfiguration : public UDataAsset
{
	GENERATED_BODY()

	public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|ID")
	EEntityID EntityID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|ID")
	EFactionID FactionID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|ID")
	EEnemyTier FactionRank = EEnemyTier::Grunt;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Configuration")
	ECombatStyle PreferredCombatStyle = ECombatStyle::Any;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Configuration")
	FCombatConfiguration CombatConfiguration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Configuration")
	TArray<FAttackConfiguration> AttackConfigurations;
};


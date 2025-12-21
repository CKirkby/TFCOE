// Created by Snow Paw Games

#pragma once

#include "CoreMinimal.h"
#include "CombatConfiguration.h"
#include "EnemyTier.h"
#include "Engine/DataAsset.h"
#include "EnemyBehaviour.generated.h"

struct FCombatConfiguration;
enum class EFactionID : uint8;
enum class EEntityID : uint8;

/**
 * 
 */
UCLASS()
class TFCOE_API UEnemyBehaviour : public UDataAsset
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
	FCombatConfiguration CombatConfiguration;
};

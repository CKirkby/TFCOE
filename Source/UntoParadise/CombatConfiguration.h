// Created by Kairo Forge

#pragma once

#include "CoreMinimal.h"
#include "FactionID.h"
#include "CombatConfiguration.generated.h"

USTRUCT(BlueprintType)
struct FCombatConfiguration
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	int Health = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	int MovementRange = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	bool bAttackerTakesTargetPriority = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	bool bFocusedAggression = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	bool bWillRetreatOnLowHealth = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	bool bWillFlank = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	EFactionID PreferredTargetFaction = EFactionID::PlayerParty;

	
};

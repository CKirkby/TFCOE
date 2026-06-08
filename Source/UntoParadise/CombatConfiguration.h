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

	// The unit will immediately select the attacking unit to be its next target of attack with a very low chance to be deterred
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	bool bAttackerTakesTargetPriority = false;

	// The unit will focus upon the current target with a very low chance to be deterred
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	bool bFocusedAggression = false;

	// NOT IMPLEMENTED: When the unit is on low health it will attempt to move away from the combat
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	bool bWillRetreatOnLowHealth = false;

	// NOT IMPLEMENTED: When the unit has its turn it will attempt to get behind the target.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	bool bWillFlank = false;

	// The units favourite faction to target, if this is set and the faction is on the field. The unit will prioritize targeting them over all else.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	EFactionID PreferredTargetFaction = EFactionID::PlayerParty;

	
};

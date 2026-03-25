// Created by Kairo Forge
#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EEnemyTier : uint8
{
	Faction_Leader UMETA(DisplayName = "Faction Leader"),
	Lieutenant UMETA(DisplayName = "Lieutenant"),
	Elite UMETA(DisplayName = "Elite"),
	Grunt UMETA(DisplayName = "Grunt"),
};
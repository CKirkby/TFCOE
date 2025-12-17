// Created by Snow Paw Games

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EFactionID : uint8
{
	PlayerParty UMETA(DisplayName = "Player Party"),
	BlackLine UMETA(DisplayName = "Black Line"),
	EmpireOfEos UMETA(DisplayName = "Empire of Eos"),
	// Monsters
	// Creatures?
};
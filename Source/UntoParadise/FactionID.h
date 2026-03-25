// Created by Kairo Forge

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EFactionID : uint8
{
	Player UMETA(DisplayName = "Player"),
	PlayerParty UMETA(DisplayName = "Player Party"),
	BlackLine UMETA(DisplayName = "Black Line"),
	EmpireOfEos UMETA(DisplayName = "Empire of Eos"),
	Forlorn UMETA(DisplayName = "Forlorn"),
	Creature UMETA(DisplayName = "Creature"),
	None UMETA(DisplayName = "None")
};
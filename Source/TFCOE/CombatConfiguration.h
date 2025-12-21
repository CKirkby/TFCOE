// Created by Snow Paw Games

#pragma once

#include "CoreMinimal.h"
#include "FactionID.h"
#include "CombatConfiguration.generated.h"

UENUM(BlueprintType)
enum ECombatType : uint8
{
	Close UMETA(DisplayName = "Close Combat"),
	Ranged UMETA(DisplayName = "Ranged Combat")
};

USTRUCT(BlueprintType)
struct FCombatConfiguration
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	TEnumAsByte<ECombatType> CombatType = Close;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	int Health = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	int MovementRange = 3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	int AttackRange = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	bool bAttackerTakesTargetPriority = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	bool bWillRetreatOnLowHealth = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	bool bWillFlank = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	EFactionID PreferredTargetFaction = EFactionID::PlayerParty;
};


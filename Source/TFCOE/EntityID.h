// Created by Snow Paw Games

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EEntityID : uint8
{
	Player UMETA(DisplayName = "Player"),
	Scavenger UMETA(DisplayName = "Scavenger"),
		
	BlackLine_Pawn UMETA(DisplayName = "BlackLine | Pawn"),
	BlackLine_Vanguard UMETA(DisplayName = "BlackLine | Vanguard"),
	BlackLine_Warden UMETA(DisplayName = "BlackLine | Warden"),
	BlackLine_Sentinel UMETA(DisplayName = "BlackLine | Sentinel"),
	BlackLine_Elite UMETA(DisplayName = "BlackLine | Elite"),
	BlackLine_Sovereign UMETA(DisplayName = "BlackLine | Sovereign"),
	BlackLine_Rook UMETA(DisplayName = "BlackLine | Rook"),
	BlackLine_Knight UMETA(DisplayName = "BlackLine | Knight"),
	BlackLine_Bishop_Enemy UMETA(DisplayName = "BlackLine | Bishop E"),
	BlackLine_Bishop_Friendly UMETA(DisplayName = "BlackLine | Bishop F"),
	BlackLine_Queen UMETA(DisplayName = "BlackLine | Queen"),
	BlackLine_King UMETA(DisplayName = "BlackLine | King")
};
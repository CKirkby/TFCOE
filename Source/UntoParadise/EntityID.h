// Created by Kairo Forge

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EEntityID : uint8
{
	Player UMETA(DisplayName = "Player"),
	Exile UMETA(DisplayName = "Exile"),
		
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
	BlackLine_King UMETA(DisplayName = "BlackLine | King"),
	
	Forlorn_Grunt UMETA(DisplayName = "Forlorn | Grunt"),
	Forlorn_Elite UMETA(DisplayName = "Forlorn | Elite"),
	Forlorn_Lieutenant UMETA(DisplayName = "Forlorn | Lieutenant"),
	Forlorn_Leader UMETA(DisplayName = "Forlorn | Leader"),
	
	Empire_Grunt UMETA(DisplayName = "Empire | Grunt"),
	Empire_Elite UMETA(DisplayName = "Empire | Elite"),
	Empire_Lieutenant UMETA(DisplayName = "Empire | Lieutenant"),
	Empire_Leader UMETA(DisplayName = "Empire | Leader"),

	Uninitialized_ID UMETA(DisplayName = "Uninitialized")
};
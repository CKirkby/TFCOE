// Created by Kairo Forge

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CombatHUD.generated.h"


UENUM(BlueprintType)
enum EUI_PlayerMode
{
	Neutral UMETA(DisplayName = "Neutral Mode"),
	Movement UMETA(DisplayName = "Movement Mode"),
	Attack UMETA(DisplayName = "Attack Mode")
};

/**
 * 
 */
UCLASS()
class TFCOE_API UCombatHUD : public UUserWidget
{
	GENERATED_BODY()

	
protected:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat HUD")
	TEnumAsByte<EUI_PlayerMode> CurrentUI_Mode = Neutral;
	
	
	UFUNCTION(BlueprintCallable, Category = "Combat HUD")
	bool CheckPriorModeActive(EUI_PlayerMode IncomingMode) const;
	
	// Getter and Setter //
	
	UFUNCTION(BlueprintCallable, Category = "Combat HUD")
	void SetPlayerMode(const EUI_PlayerMode NewMode)
	{
		CurrentUI_Mode = NewMode;
	}
	
	UFUNCTION(BlueprintCallable, Category = "Combat HUD")
	EUI_PlayerMode GetPlayerMode() const
	{
		return CurrentUI_Mode;
	}
};

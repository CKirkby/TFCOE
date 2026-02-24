// Created by Snow Paw Games

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CombatHUD.generated.h"

/**
 * 
 */
UCLASS()
class TFCOE_API UCombatHUD : public UUserWidget
{
	GENERATED_BODY()
	
	virtual void NativeConstruct() override;
	
};

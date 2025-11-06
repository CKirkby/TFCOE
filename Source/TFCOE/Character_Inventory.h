// Created by Snow Paw Games

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Character_Inventory.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TFCOE_API UCharacter_Inventory : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCharacter_Inventory();

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	TMap<FName, int> GeneralItems = {};

	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	TMap<FName, int> KeyItems = {};

public:
	// Getters and Setters // 
	
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	TMap<FName, int> GetGeneralItemList() const
	{
		return GeneralItems;
	}

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	TMap<FName, int> GetKeyItemList() const
	{
		return KeyItems;
	}

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void AddToGeneralItems(const FName KeyID, const int ItemCount);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void AddToKeyItems(const FName KeyID, const int ItemCount)
	{
		KeyItems.Add(KeyID, ItemCount);
	}
};

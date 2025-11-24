// Created by Snow Paw Games

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CharacterCombatData.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TFCOE_API UCharacterCombatData : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCharacterCombatData();

protected:

	// Movement
	FVector2D CurrentGridCoordinates = FVector2D::ZeroVector;
	
	// Called when the game starts
	virtual void BeginPlay() override;

public:	


	// Getter and Setter
	
	UFUNCTION(BlueprintCallable, Category = "Player")
	FVector2D GetCurrentGridCoordinates() const
	{
		return CurrentGridCoordinates;
	}

	UFUNCTION(BlueprintCallable, Category = "Player")
	void SetCurrentGridCoordinates(const FVector2D Coordinates)
	{
		CurrentGridCoordinates = Coordinates;
	}
};

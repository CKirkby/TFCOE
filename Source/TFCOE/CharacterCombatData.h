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

	// Action Points
	int TimePoints = 10;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	int MaxTimePoints = 10;
	
	// Movement
	FVector2D CurrentGridCoordinates = FVector2D::ZeroVector;

	virtual void BeginPlay() override;

public:
	
	UFUNCTION(BlueprintCallable, Category="CombatManager")
	bool CheckCanAffordMovement(FVector2D CurrentCoordinates, FVector2D TargetCoordinates);

	int CalculateMovementCost(FVector2D CurrentCoordinates, const FVector2D TargetCoordinates);
	
	// Getter and Setter //
	
	UFUNCTION(BlueprintCallable, Category = "CombatData")
	FVector2D GetCurrentGridCoordinates() const
	{
		return CurrentGridCoordinates;
	}

	UFUNCTION(BlueprintCallable, Category = "CombatData")
	void SetCurrentGridCoordinates(const FVector2D Coordinates)
	{
		CurrentGridCoordinates = Coordinates;
	}

	UFUNCTION(BlueprintCallable, Category = "CombatData")
	int GetTimePoints() const
	{
		return TimePoints;
	}

	UFUNCTION(BlueprintCallable, Category = "CombatData")
	void SetTimePoints(const int NewTimePoints)
	{
		TimePoints = NewTimePoints;
	}

	UFUNCTION(BlueprintCallable, Category = "CombatData")
	void ResetTimePoints()
	{ 
		TimePoints = MaxTimePoints;
	}
};

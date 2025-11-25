// Created by Snow Paw Games

#include "CharacterCombatData.h"

UCharacterCombatData::UCharacterCombatData()
{
	PrimaryComponentTick.bCanEverTick = false;
}


void UCharacterCombatData::BeginPlay()
{
	Super::BeginPlay();

	
	TimePoints = MaxTimePoints;
}

int UCharacterCombatData::CalculateMovementCost(const FVector2D CurrentCoordinates,
                                                const FVector2D TargetCoordinates)
{
	// Uses the Chebyshev method to calculate action cost.

	// Gets the absolute number because I don't care the direction of movement only the distance.
	const float DeltaX = FMath::Abs(TargetCoordinates.X - CurrentCoordinates.X);
	const float DeltaY = FMath::Abs(TargetCoordinates.Y - CurrentCoordinates.Y);

	// Gets the max number between the two variables so it can calculate the max amount of cost on the axis.
	return FMath::Max(DeltaX, DeltaY);
}

bool UCharacterCombatData::CheckCanAffordMovement(const FVector2D CurrentCoordinates, const FVector2D TargetCoordinates)
{
	const int CostToMove = CalculateMovementCost(CurrentCoordinates, TargetCoordinates);

	// Checks if both the combatant has time points left and also the cost doesn't exceed the amount left.
	if (TimePoints > 0 && CostToMove <= TimePoints)
	{
		// Removes the time points from the pool
		SetTimePoints(FMath::Clamp(TimePoints - CostToMove, 0, MaxTimePoints));
		return true;
	}
	
	return false;
}

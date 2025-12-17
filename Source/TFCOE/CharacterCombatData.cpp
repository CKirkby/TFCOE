// Created by Snow Paw Games

#include "CharacterCombatData.h"

#include "BoardPiece.h"
#include "CombatInterface.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "GameFramework/GameMode.h"

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

bool UCharacterCombatData::CheckShouldMove(FVector2D PlayerCoordinates)
{
	
	return false;
}

FVector2D UCharacterCombatData::CalculateTargetMovementPiece() const
{
	ICombatInterface* CombatInterface = Cast<ICombatInterface>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (!CombatInterface) return FVector2D(1,1);

	ICombatInterface* CombatInterfaceGamemode = Cast<ICombatInterface>(UGameplayStatics::GetGameMode(GetWorld()));
	if (!CombatInterfaceGamemode) return FVector2D(1,1);

	const FVector2D PlayerCoordinates = CombatInterface->GetGridCoordinates();
	FVector2D TargetCoordinates = PlayerCoordinates;

	const FVector2D DirToPlayer = FVector2D(PlayerCoordinates.X - CurrentGridCoordinates.X, PlayerCoordinates.Y - CurrentGridCoordinates.Y);

	// Calculates which piece should be the target of this actor. 
	if (FMath::Abs(DirToPlayer.X) > FMath::Abs(DirToPlayer.Y))
	{
		if (DirToPlayer.X < 0)
		{
			// Player is to the left of this enemy, move target to the right of player.
			TargetCoordinates.X += 1;
		}
		else
		{
			// Player is to the right of this enemy, move target to the left of player.
			TargetCoordinates.X -= 1;
		}

		// If it has chosen this route, get the piece coordinates and return them to the function. (WHAT IF IT CANNOT MOVE AT ALL? IS THIS FLAWED)
		AActor* TargetPiece = CombatInterfaceGamemode->GetGridPieceFromCoordinates(TargetCoordinates);
		if (!TargetPiece) return FVector2D(1,1);

		// Gets the interface of the piece and makes sure it is a piece that can be moved to.
		if (ICombatInterface* CombatInterfaceChosenPiece = Cast<ICombatInterface>(TargetPiece))
		{
			if (CombatInterfaceChosenPiece->GetCurrentPieceState() == EPieceState::Enabled)
			{
				return TargetCoordinates;
			}
			else
			{
				// DO SOMETHING? TRY AGAIN? DO SOMETHING
			}
		}
	}
	else
	{
		if (DirToPlayer.Y < 0)
		{
			// The Player is below this enemy, move target to above the player.
			TargetCoordinates.Y += 1;
		}
		else
		{
			// The Player is Above this enemy, move target to below the player.
			TargetCoordinates.Y -= 1;
		}

		// If it has chosen this route, get the piece coordinates and return them to the function. (WHAT IF IT CANNOT MOVE AT ALL? IS THIS FLAWED)
		AActor* TargetPiece = CombatInterfaceGamemode->GetGridPieceFromCoordinates(TargetCoordinates);
		if (!TargetPiece) return FVector2D(1,1);

		// Gets the interface of the piece and makes sure it is a piece that can be moved to.
		if (ICombatInterface* CombatInterfaceChosenPiece = Cast<ICombatInterface>(TargetPiece))
		{
			if (CombatInterfaceChosenPiece->GetCurrentPieceState() == EPieceState::Enabled)
			{
				return TargetCoordinates;
			}
			else
			{
				// DO SOMETHING? TRY AGAIN? DO SOMETHING
			}
		}
	}
	
	return FVector2D(1,1);
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

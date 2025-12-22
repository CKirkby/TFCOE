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

void UCharacterCombatData::ExecuteCurrentTurn()
{
	// Step 1: Select Target for this turn.
	CurrentTarget = SelectTargetForTurn();
	if (!CurrentTarget) return;

	// FOR TESTING //
	UE_LOG(LogTemp, Error, TEXT("Current Target is: %s"), *CurrentTarget->GetName());
	
	// Step 2: Check if the actor should move

	// End Current Turn
}

AActor* UCharacterCombatData::SelectTargetForTurn()
{	
	// Gets preferred target faction from config
	const EFactionID PreferredFaction = EntityCombatConfiguration->CombatConfiguration.PreferredTargetFaction;

	// Gets the main game modes interface
	ICombatInterface* CombatInterfaceGamemode = Cast<ICombatInterface>(UGameplayStatics::GetGameMode(GetWorld()));
	ICombatInterface* CombatInterfacePlayer = Cast<ICombatInterface>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (!CombatInterfaceGamemode) return nullptr;
	if (!CombatInterfacePlayer) return nullptr;
	
	// Gets the current combats combatants from the gamemode
	TArray<AActor*> CachedActiveCombatants = CombatInterfaceGamemode->GetActiveCombatantRoster();
	if (CachedActiveCombatants.IsEmpty()) return CombatInterfacePlayer->GetPlayerCombatant();
	
	// Checks to make sure the owner of this isn't in the list of possible targets
 	if (CachedActiveCombatants.Contains(this->GetOwner()))
	{
		CachedActiveCombatants.Remove(this->GetOwner());
	}
	
	// Creates an array of potential targets to choose from. 
	TArray<AActor*> PotentialTargets = {};

	// Logic Begin //
	
	// The Actor was attacked last turn
	if (AttackedLastTurn)
	{
		// This actor is set to prioritise the attackers so it will automatically target those.
  		if (EntityCombatConfiguration->CombatConfiguration.bAttackerTakesTargetPriority)
		{
			// Returns the previous attacker as the new target if this entity is set to prioritise those attackers. 
			if (PreviousAttacker)
			{
				// TODO - Make function to set previous attacker
				return PreviousAttacker;
			}
			UE_LOG(LogTemp, Warning, TEXT("Combat Data: Select Target For Turn - No previous Attacker ref set. Auto setting to player combatant"))
			return CombatInterfacePlayer->GetPlayerCombatant();
		}

		// Chance to keep attacking target, If the entity has focused aggression, very little chance to change target, otherwise normal chance
		float ChanceToChangeTarget = EntityCombatConfiguration->CombatConfiguration.bFocusedAggression ? 0.05F : 0.30f;
		bool bShouldChangeTarget = FMath::FRand() < ChanceToChangeTarget;
		if (CurrentTarget && !bShouldChangeTarget)
		{
			// Target Current Target
			return CurrentTarget;
		}

		// Makes sure this entity actually has a preferred faction to target and if not it will prioritise others.
		
		if (PreferredFaction != EFactionID::None)
		{
			// Gets the targets of these actors preferred target faction. Also checks that there are faction members of present. Otherwise this will return empty.
			PotentialTargets = GetCombatantsByFaction(CachedActiveCombatants, PreferredFaction);

			// Chooses either the closest faction member or random. Weight: Closest 70% / Random 30%
			if (!PotentialTargets.IsEmpty())
			{
				if ( AActor* PotentialFactionTarget = GetTargetFromClosestOrRandom(PotentialTargets, 0.70f))
				{
					return PotentialFactionTarget;
				}
			}

			// None of the preferred faction is available so just go after the closest or random //
			
			// Chooses either the closest member or random. Weight: Closest 70% / Random 30%
			if (AActor* PotentialTarget = GetTargetFromClosestOrRandom(CachedActiveCombatants, 0.70f))
			{
				return PotentialTarget;
			}

			// If all this fails for whatever reason, just target the player.
			return CombatInterfacePlayer->GetPlayerCombatant();
		}

		// The Preferred Faction is none //
		
		// Target Player or Players party with weight 80/20 depending on the last attacker.
		if (FMath::FRand() < 0.80f)
		{
			// Target Player
			return CombatInterfacePlayer->GetPlayerCombatant();
		}

		// Target Party //
		// Gets the party members from the active combatants.
		PotentialTargets = GetCombatantsByFaction(CachedActiveCombatants, EFactionID::PlayerParty);
		if (!PotentialTargets.IsEmpty())
		{
			const int RandIndex = FMath::RandRange(0, PotentialTargets.Num() - 1);
			return PotentialTargets[RandIndex];
		}
	}
	
	// Was not attacked last turn, choose a target.
  	float ChanceToChangeTarget = EntityCombatConfiguration->CombatConfiguration.bFocusedAggression ? 0.05F : 0.15f;
	bool bShouldChangeTarget = FMath::FRand() < ChanceToChangeTarget;
	if (CurrentTarget && !bShouldChangeTarget)
	{
		// Target Current Target
		return CurrentTarget;
	}

	// Else should either change target / select new target //

	if (PreferredFaction != EFactionID::None)
	{
		// Try looking for preferred faction. 
		PotentialTargets = GetCombatantsByFaction(CachedActiveCombatants, PreferredFaction);

		// Chooses either the closest faction member or random. Weight: Closest 70% / Random 30%
		if (AActor* PotentialTarget = GetTargetFromClosestOrRandom(PotentialTargets, 0.70f))
		{
			return PotentialTarget;
		}
	}
	
	// Else if no preferred faction target closest or player
	// Target Player or Players party with weight 80/20 depending on the last attacker.
	if (FMath::FRand() < 0.80f)
	{
		// Target Player
		return CombatInterfacePlayer->GetPlayerCombatant();
	}

	// Target Party
	// Gets the party members from the active combatants.
	PotentialTargets = GetCombatantsByFaction(CachedActiveCombatants, EFactionID::PlayerParty);
	if (!PotentialTargets.IsEmpty())
	{
		const int RandIndex = FMath::RandRange(0, PotentialTargets.Num() - 1);
		return PotentialTargets[RandIndex];
	}

	// Complete Function Fail
	UE_LOG(LogTemp, Error, TEXT("Combat Data: Select Target for Turn - Complete function fail"))
	return nullptr;
}

bool UCharacterCombatData::CheckShouldMove(FVector2D PlayerCoordinates)
{
	
	return false;
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
				// TODO - Make function to get different target if the piece is not usable
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
				// TODO - Make function to get different target if the piece is not usable
			}
		}
	}
	
	return FVector2D(1,1);
}

EFactionID UCharacterCombatData::GetFactionID() const
{
	if (EntityCombatConfiguration)
	{
		return EntityCombatConfiguration->FactionID;
	}

	UE_LOG(LogTemp, Error, TEXT("Combat Data: Get Faction ID - No Combat Configuration Set"))
	return EFactionID::None;
}

EEnemyTier UCharacterCombatData::GetFactionRank() const
{
	if (EntityCombatConfiguration)
	{
		return EntityCombatConfiguration->FactionRank;
	}

	UE_LOG(LogTemp, Error, TEXT("Combat Data: Get Faction Rank - No Combat Configuration Set"))
	return EEnemyTier::Grunt;
}

TArray<AActor*> UCharacterCombatData::SortCombatantsByDistance(const TArray<AActor*>& Combatants) const
{
	if (Combatants.IsEmpty()) return TArray<AActor*>();
	
	// Creates a new array based from the combatants to be sorted through
	TArray<AActor*> SortedCombatants = Combatants;
	TMap<AActor*, int> CachedDistances;

	// Sorts through the array to get the current distances and stores the distances in the map
	for (auto Combatant : SortedCombatants)
	{
		// Gets the interface for each combatant
		ICombatInterface* CombatInterface = Cast<ICombatInterface>(Combatant);
		if (!CombatInterface) continue;

		// Gets the current coordinates from the combatants
		FVector2D Coords = CombatInterface->GetGridCoordinates();

		// Calculates the distance in grid pieces from the current entity to these combatants and then stores them. 
		CachedDistances.Add(Combatant, FMath::Abs(Coords.X - CurrentGridCoordinates.X) + FMath::Abs(Coords.Y - CurrentGridCoordinates.Y));
	}

	// Once the distances have been calculated and stored, it will sort the array from closest to furthest and returns it.
	SortedCombatants.Sort([&CachedDistances](const AActor& A, const AActor& B)
	{
		return CachedDistances[&A] < CachedDistances[&B];
	});

	return SortedCombatants;
}

TArray<AActor*> UCharacterCombatData::GetCombatantsByFaction(TArray<AActor*> CombatantsToCheck, const EFactionID FactionToCheck)
{
	if (CombatantsToCheck.IsEmpty()) return TArray<AActor*>();

	// Creates an array to store the potential faction targets.
	TArray<AActor*> PotentialTargets = {};

	// Cycles through the combatants and checks their faction ID against the preferred faction
	for (auto Combatant : CombatantsToCheck)
	{
		ICombatInterface* CombatInterfaceCombatant = Cast<ICombatInterface>(Combatant);
		if (!CombatInterfaceCombatant) continue;

		// Gets a combatants faction ID
		EFactionID CombatantsFaction = CombatInterfaceCombatant->GetActorFactionID();

		// Checks the faction ID against the preferred faction
		if (CombatantsFaction == FactionToCheck)
		{
			PotentialTargets.Add(Combatant);
		}
	}

	if (!PotentialTargets.IsEmpty())
	{
		return PotentialTargets;
	}

	return TArray<AActor*>();
}

AActor* UCharacterCombatData::GetTargetFromClosestOrRandom(TArray<AActor*> PotentialTargets, const float Weight) const
{
	if (PotentialTargets.IsEmpty()) return nullptr;
	
	// Chooses either the closest faction member or random. Weight: Closest 70% / Random 30%
	if (FMath::FRand() < Weight)
	{
		// Target Closest Enemy
		TArray<AActor*> DistanceSortedPotentialTargets = SortCombatantsByDistance(PotentialTargets);
		return DistanceSortedPotentialTargets[0];
	}
			
	// Target Random Enemy. Gets a random enemy and sets that to be the target#
	if (!PotentialTargets.IsEmpty())
	{
		const int RandIndex = FMath::RandRange(0, PotentialTargets.Num() - 1);
		return PotentialTargets[RandIndex];
	}

	UE_LOG(LogTemp, Error, TEXT("Combat Data: Get Target From Closest or Random - Complete function fail"))
	return nullptr;
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

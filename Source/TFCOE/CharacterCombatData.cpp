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

	// Stores a reference to the game mode interface and player
	CombatInterfaceGamemode = Cast<ICombatInterface>(UGameplayStatics::GetGameMode(GetWorld()));
	CombatInterfacePlayer = Cast<ICombatInterface>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
}

void UCharacterCombatData::ExecuteCurrentTurn()
{
	// Step 1: Select Target for this turn.
	CurrentTarget = SelectTargetForTurn();
	if (!CurrentTarget)
	{
		EndThisActorTurn();
		return;
	}

	// FOR TESTING STEP 1 //
	UE_LOG(LogTemp, Error, TEXT("Current Target is: %s"), *CurrentTarget->GetName());

	// Step 2: Choose an attack
	FAttackConfiguration* CurrentAttack = ChooseAttackForTurn(CurrentTarget);
	if (!CurrentAttack)
	{
		EndThisActorTurn();
		return;
	}

	// TESTING STEP 2 //
	UE_LOG(LogTemp, Error, TEXT("Chosen Attack: %s"), *CurrentAttack->AttackID.ToString());
	
	// Step 3: Check if the actor should move
	if (CheckShouldMove(CurrentAttack, CurrentTarget))
	{
		// Step 4: Movement
		const FIntPoint TargetCoordinatesForMove = ChooseMovementPosition(CurrentTarget);
		UE_LOG(LogTemp, Error, TEXT("Movement Target: %i, %i"), TargetCoordinatesForMove.X, TargetCoordinatesForMove.Y);

		// Move
	}
	else
	{
		// Step 5: Attack
	}
	
	// Final Step: End Turn w Delay
	DelayLambda(1.0f, [this]()
	{
		// End Current Turn
		EndThisActorTurn();
	});
}

AActor* UCharacterCombatData::SelectTargetForTurn()
{	
	// TODO - Chunky function needs to be made into helper functions
	
	if (!CombatInterfaceGamemode) return nullptr;
	if (!CombatInterfacePlayer) return nullptr;

	AActor* PlayerCombatant = CombatInterfacePlayer->GetPlayerCombatant();
	if (!PlayerCombatant) return nullptr;

	// Checks if the characters combat config settings are present, if not just default target the player.
	if (!EntityCombatConfiguration)
	{
		UE_LOG(LogTemp, Error, TEXT("Combat Data: Select Target for Turn - Combat Configuration Missing / Not set"))
		return PlayerCombatant;
	}
	
	// Gets preferred target faction from config
	const EFactionID PreferredFaction = EntityCombatConfiguration->CombatConfiguration.PreferredTargetFaction;
	
	// Gets the current combats combatants from the gamemode
	TArray<AActor*> CachedActiveCombatants = CombatInterfaceGamemode->GetActiveCombatantRoster();
	if (CachedActiveCombatants.IsEmpty()) return PlayerCombatant;
	
	// Checks to make sure the owner of this isn't in the list of possible targets
	CachedActiveCombatants.Remove(this->GetOwner());
	
	// This acts as a failsafe, for whatever reason if the active combatants is empty after removing the current turns' actor. It won't crash. 
	if (CachedActiveCombatants.IsEmpty()) return PlayerCombatant;
	
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
			return PlayerCombatant;
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
			return PlayerCombatant;
		}

		// The Preferred Faction is none //
		
		// Target Player or Players party with weight 80/20 depending on the last attacker.
		if (FMath::FRand() < 0.80f)
		{
			// Target Player
			return PlayerCombatant;
		}

		// Target Party //
		// Gets the party members from the active combatants.
		PotentialTargets = GetCombatantsByFaction(CachedActiveCombatants, EFactionID::PlayerParty);
		if (!PotentialTargets.IsEmpty())
		{
			const int RandIndex = FMath::RandRange(0, PotentialTargets.Num() - 1);
			return PotentialTargets[RandIndex];
		}

		// If all of this functionality fails, default to targeting the player character.
		return PlayerCombatant;
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
		if (!PotentialTargets.IsEmpty())
		{
			if (AActor* PotentialTarget = GetTargetFromClosestOrRandom(PotentialTargets, 0.70f))
			{
				return PotentialTarget;
			}
		}
	}
	
	// Else if no preferred faction target closest or player //
	
	// Get whoever is closest from player and companions then choose them.
	// Gets the party members from the active combatants.
	PotentialTargets = GetCombatantsByFaction(CachedActiveCombatants, EFactionID::PlayerParty);
	PotentialTargets.Add(PlayerCombatant);

	if (!PotentialTargets.IsEmpty())
	{
		if (AActor* PotentialTarget = GetTargetFromClosestOrRandom(PotentialTargets, 0.80f))
		{
			return PotentialTarget;
		}

		// If the actor fails, then just get a random one from the index.
		const int RandIndex = FMath::RandRange(0, PotentialTargets.Num() - 1);
		return PotentialTargets[RandIndex];
	}
	
	// If all of this functionality fails, default to targeting the player character.
	return PlayerCombatant;
}

bool UCharacterCombatData::CheckShouldMove(const FAttackConfiguration* ChosenAttack, AActor* ChosenTarget)
{
	if (!EntityCombatConfiguration || !ChosenAttack || !ChosenTarget)
	{
		UE_LOG(LogTemp, Error, TEXT("Combat Data: CheckShouldMove - Reference failure"));
		return false;
	}

	ICombatInterface* CombatInterfaceTarget = Cast<ICombatInterface>(ChosenTarget);
	if (CombatInterfaceTarget == nullptr) return false;
	
	FIntPoint TargetCoordinates = CombatInterfaceTarget->GetGridCoordinates();
	int DistToTarget = GetGridDistanceAllDir(CurrentGridCoordinates, TargetCoordinates);

	// Checks if this attack requires alignment, if it does then it will immediately tell it to move
	if (ChosenAttack->RequiresAlignment)
	{
		if (!IsAlignedAllDir(CurrentGridCoordinates, TargetCoordinates))
		{
			return true;
		}
	}

	// If it is already aligned or otherwise doesn't require alignment, then it will continue 
	switch (ChosenAttack->AttackType)
	{
	case EAttackType::Close:
		{
			const int AttackRange = ChosenAttack->AttackRange;
			return DistToTarget > AttackRange;
		}
	
	case EAttackType::Ranged:
		{
			const int MinRange = ChosenAttack->AttackRange;
			const int MaxRange = ChosenAttack->MaxAttackRange;
			return (DistToTarget < MinRange) || (DistToTarget > MaxRange);
		}
	}
	
	return false;
}

FAttackConfiguration* UCharacterCombatData::ChooseAttackForTurn(AActor* TargetActor) const
{
	if (!TargetActor || !EntityCombatConfiguration || EntityCombatConfiguration->AttackConfigurations.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("Combat Data: Choose Attack - Reference fail"));
		return nullptr;
	}

	// Gets the targets interface to get the grid coordinates.
	ICombatInterface* CombatInterfaceTarget = Cast<ICombatInterface>(TargetActor);
	if (!CombatInterfaceTarget) return nullptr;

	// Gets the targets coordinates. 
	const FIntPoint TargetCoordinates = CombatInterfaceTarget->GetGridCoordinates();

	// Gets these actors preferred combat style. 
	const ECombatStyle PreferredCombatStyle = EntityCombatConfiguration->PreferredCombatStyle;
	const int DistToTarget = GetGridDistanceAllDir(CurrentGridCoordinates, TargetCoordinates);

	// Creates an attack type to chose based on distance. If this actor is far, use ranged, if not move close. 
	EAttackType TargetAttackToUse = (DistToTarget > 1) ? EAttackType::Ranged : EAttackType::Close;

	// Considers if the actor has a preferred style of attack. Will usually only be close or ranged.
	switch (PreferredCombatStyle)
	{
	case ECombatStyle::Any:
		break;
	
	case ECombatStyle::PreferClose:
		// 90% Chance to be a close attack
		if (FMath::FRand() < 0.90f) TargetAttackToUse = EAttackType::Close;
		break;
		
	case ECombatStyle::PreferRanged:
		// 90% Chance to be a ranged attack
		if (FMath::FRand() < 0.90f) TargetAttackToUse = EAttackType::Ranged;
		break;
	}

	// After that, it will get attempt to get a random attack from the priority list, if there is no attacks it will just choose at random.
	return GetAttackFromType(TargetAttackToUse);
}

FAttackConfiguration* UCharacterCombatData::GetAttackFromType(const EAttackType AttackType) const
{
	if (EntityCombatConfiguration->AttackConfigurations.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("Combat Data - Get Attack from type - No attack configurations available"))
		return nullptr;
	}
	
	// Caches the attacks this actor has to sort through. 
	TArray<FAttackConfiguration>& CachedAttacks = EntityCombatConfiguration->AttackConfigurations;
	if (CachedAttacks.IsEmpty()) return nullptr;

	TArray<FAttackConfiguration*> DesiredAttacks;

	// Sorts through the attacks to add the priority attacks to be returned. 
	for (FAttackConfiguration& Attack : CachedAttacks)
	{
		if (Attack.AttackType == AttackType)
		{
			DesiredAttacks.Add(&Attack);
		}
	}

	// If it's not empty, that means preferred attacks do exist and uses them as a priority.
	if (!DesiredAttacks.IsEmpty())
	{
		const int RandIndex = FMath::RandRange(0, DesiredAttacks.Num() - 1);
		return DesiredAttacks[RandIndex];
	}

	// Choose any attack.
	const int RandIndex = FMath::RandRange(0, CachedAttacks.Num() - 1);
	return &CachedAttacks[RandIndex];
}

FIntPoint UCharacterCombatData::ChooseMovementPosition(AActor* TargetActor)
{
	if (!TargetActor || !EntityCombatConfiguration)
	{
		UE_LOG(LogTemp, Error, TEXT("Combat Data: Choose Movement Position - Reference fail"));
		return CurrentGridCoordinates;
	}

	// Gets the interface for the target
	ICombatInterface* CombatInterfaceTarget = Cast<ICombatInterface>(TargetActor);
	if (!CombatInterfaceTarget) return CurrentGridCoordinates;

	// Gets the coordinates of the grid the target is stood on
   	FIntPoint TargetCoordinates = CombatInterfaceTarget->GetGridCoordinates();
	int MovementRange = EntityCombatConfiguration->CombatConfiguration.MovementRange;

	// Sets an array to check whether 
	TArray<FIntPoint> CoordinatesToAttempt;
	TArray<FIntPoint> SuccessfulCandidates;

	// Populates the array with the grid pieces around the 
	GetGridAdjacentAllDir(TargetCoordinates, CoordinatesToAttempt);
	if (CoordinatesToAttempt.IsEmpty()) return CurrentGridCoordinates;

	// Sorts through the coordinates to check to see which
	for (auto Coordinates : CoordinatesToAttempt)
	{
		if (!DoesGridCoordinatesExist(Coordinates)) continue;
		if (!IsGridPieceActive(Coordinates)) continue;

		// Culls the non-existent or occupied coordinates. 
		SuccessfulCandidates.Add(Coordinates);
	}

	if (SuccessfulCandidates.IsEmpty()) return CurrentGridCoordinates;

	// Creates a struct to store how the reachable grid points and how many steps it will take to get there.
	struct FCandidatePathway
	{
		FIntPoint GridPoint;
		int Steps = 0;
	};

	// Creates an array of those structs to prepare for storage
	TArray<FCandidatePathway> ReachableGridPositions;
	ReachableGridPositions.Reserve(SuccessfulCandidates.Num());

	// Calculates a path to the position and how many steps it will take to reach that. Then store it in the array.
	for (FIntPoint& Candidate : SuccessfulCandidates)
	{
		int Steps = 0;
		if (CalculatePathToPosition(CurrentGridCoordinates, Candidate, Steps))
		{
			if (Steps <= MovementRange)
			{
				ReachableGridPositions.Add({Candidate, Steps});
			}
		}
	}

	if (ReachableGridPositions.IsEmpty()) return CurrentGridCoordinates;

	// Sorts the structs based on how less steps it will take to reach, the less, the lower in the array.
	ReachableGridPositions.Sort([](const FCandidatePathway& PathA, const FCandidatePathway& PathB)
	{
		if (PathA.Steps != PathB.Steps) return PathA.Steps < PathB.Steps;
		if (PathA.GridPoint.X != PathB.GridPoint.X) return PathA.GridPoint.X < PathB.GridPoint.X;
		return PathA.GridPoint.Y < PathB.GridPoint.Y;
	});

	// Returns the struct with the least amount of steps,
	return ReachableGridPositions[0].GridPoint;
}

int UCharacterCombatData::CalculateMovementCost(const FIntPoint CurrentCoordinates,
                                                const FIntPoint TargetCoordinates)
{
	// Uses the Chebyshev method to calculate action cost.

	// Gets the absolute number because I don't care the direction of movement only the distance.
	const int DeltaX = FMath::Abs(TargetCoordinates.X - CurrentCoordinates.X);
	const int DeltaY = FMath::Abs(TargetCoordinates.Y - CurrentCoordinates.Y);

	// Gets the max number between the two variables so it can calculate the max amount of cost on the axis.
	return FMath::Max(DeltaX, DeltaY);
}

FIntPoint UCharacterCombatData::CalculateTargetMovementPiece() const
{
	if (!CombatInterfacePlayer) return FIntPoint(1,1);
	
	if (!CombatInterfaceGamemode) return FIntPoint(1,1);

	const FIntPoint PlayerCoordinates = CombatInterfacePlayer->GetGridCoordinates();
	FIntPoint TargetCoordinates = PlayerCoordinates;

	const FIntPoint DirToPlayer = FIntPoint(PlayerCoordinates.X - CurrentGridCoordinates.X, PlayerCoordinates.Y - CurrentGridCoordinates.Y);

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
		if (!TargetPiece) return FIntPoint(1,1);

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
		if (!TargetPiece) return FIntPoint(1,1);

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
	
	return FIntPoint(1,1);
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
		const FIntPoint Coords = CombatInterface->GetGridCoordinates();

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

void UCharacterCombatData::DelayLambda(const float DelayTime, TFunction<void()> Function)
{
	TWeakObjectPtr<UCharacterCombatData> SafeThis = this;
	FTimerHandle TimerHandle;

	UWorld* World = GetWorld();
	if (!World) return;

	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [SafeThis, Function]()
	{
		if (!SafeThis.IsValid()) return;

		Function();
		
	},DelayTime, false);
}

bool UCharacterCombatData::CheckCanAffordMovement(const FIntPoint CurrentCoordinates, const FIntPoint TargetCoordinates)
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

void UCharacterCombatData::EndThisActorTurn() const
{
	// Tells the gamemode that this actor has finished its turn and to then cycle to the next enemy turn.
	if (CombatInterfaceGamemode)
	{
		CombatInterfaceGamemode->NotifyEndIndividualTurn();
	}
}

bool UCharacterCombatData::CheckIsAdjacent(FIntPoint& PointA, FIntPoint& PointB) const
{
	// Gets the absolute value of these and checks that neither are 1, because that means the actor is next to the target.
	return GetGridDistanceAllDir(PointA, PointB) == 1;
}

int UCharacterCombatData::GetGridDistanceAllDir(const FIntPoint& PointA, const FIntPoint& PointB) const
{
	// Uses the Chebyshev method to include Diagonals into the movement consideration
	return FMath::Max(FMath::Abs(PointA.X - PointB.X), FMath::Abs(PointA.Y - PointB.Y));
}

int UCharacterCombatData::GetGridDistanceCardinal(const FIntPoint& PointA, const FIntPoint& PointB) const
{
	// Uses the Manhattan method to check distance using only the up,down,left,right directions. 
	return FMath::Abs(PointA.X - PointB.X) + FMath::Abs(PointA.Y - PointB.Y);
}

bool UCharacterCombatData::IsAlignedCardinal(FIntPoint& PointA, FIntPoint& PointB) const
{
	return (PointA.X == PointB.X || PointA.Y == PointB.Y);
}

bool UCharacterCombatData::IsAlignedAllDir(FIntPoint& PointA, FIntPoint& PointB) const
{
	int DeltaX = FMath::Abs(PointA.X - PointB.X);
	int DeltaY = FMath::Abs(PointA.Y - PointB.Y);

	return (DeltaX == 0) || (DeltaY == 0) || (DeltaX == DeltaY);
}

void UCharacterCombatData::GetGridAdjacentAllDir(const FIntPoint& OriginCoordinates,
	TArray<FIntPoint>& OutNeighbors) const
{
	OutNeighbors.Reset(8);
	OutNeighbors.Add(OriginCoordinates + FIntPoint(1, 0));
	OutNeighbors.Add(OriginCoordinates + FIntPoint(-1, 0));
	OutNeighbors.Add(OriginCoordinates + FIntPoint(0, 1));
	OutNeighbors.Add(OriginCoordinates + FIntPoint(0, -1));
	OutNeighbors.Add(OriginCoordinates + FIntPoint(1, 1));
	OutNeighbors.Add(OriginCoordinates + FIntPoint(1, -1));
	OutNeighbors.Add(OriginCoordinates + FIntPoint(-1, 1));
	OutNeighbors.Add(OriginCoordinates + FIntPoint(-1, -1));
}

bool UCharacterCombatData::DoesGridCoordinatesExist(const FIntPoint GridCoordinates) const
{
	return CombatInterfaceGamemode->DoesGridContainCoordinate(GridCoordinates);
}

bool UCharacterCombatData::IsGridPieceActive(const FIntPoint GridCoordinates) const
{
	//Gets the grid piece so it can check the specific piece for its state. 
	AActor* GridPiece = CombatInterfaceGamemode->GetGridPieceFromCoordinates(GridCoordinates);

	// Gets its interface.
	ICombatInterface* CombatInterfaceGridPiece = Cast<ICombatInterface>(GridPiece);
	if (!CombatInterfaceGridPiece) return false;
	
	// Checks the piece isn't occupied or non-walkable. 
	if (CombatInterfaceGridPiece->GetCurrentPieceState() == EPieceState::Enabled)
	{
		return true;
	}

	// Otherwise it will be not available and must not be chosen.
	return false;
}

bool UCharacterCombatData::CalculatePathToPosition(const FIntPoint& Start, const FIntPoint& Target, int& OutSteps) const
{
	// Tries to calculate a path to the target, based on the least amount of steps it will take to reach.
	TArray<FIntPoint> Path;
	if (!FindPathUsingBFS(Start, Target, Path))
	{
		return false;
	}

	// Takes the amount of steps, considers the start is the actors location so -1 step.
	OutSteps = FMath::Max(0, Path.Num() - 1);
	return true;
}

bool UCharacterCombatData::FindPathUsingBFS(const FIntPoint& StartCoords, const FIntPoint& TargetCoords, TArray<FIntPoint>& OutPath) const
{
	OutPath.Empty();

	// Failsafe incase it will try to move towards itself.
	if (StartCoords == TargetCoords)
	{
		OutPath.Add(StartCoords);
		return true;
	}

	TQueue<FIntPoint> PathQueue; // A queue of grid points to try to access for reachability.
	TSet<FIntPoint> Finished; // Which grid points have been tested
	TMap<FIntPoint, FIntPoint> PreviousAttempt; // The previous to current attempt.

	// Starts out with the starting coordinates.
	PathQueue.Enqueue(StartCoords);
	Finished.Add(StartCoords);

	// An array to contain the adjacent grid positions to the target we are testing.
	TArray<FIntPoint> AdjacentPoints;
	AdjacentPoints.Reserve(8);

	// Iterates through all the neighbors until it will find an eventual path to the target.
	while (!PathQueue.IsEmpty())
	{
		FIntPoint CurrentAttemptCoords;
		PathQueue.Dequeue(CurrentAttemptCoords);

		// Gets all the adjacent grid positions to the target and populates the array with them.
		GetGridAdjacentAllDir(CurrentAttemptCoords, AdjacentPoints);

		for (FIntPoint GridPoint : AdjacentPoints)
		{
			// Checks that it exists, it is accessible and it hasn't been tested yet.
			if (Finished.Contains(GridPoint)) continue;
			if (!DoesGridCoordinatesExist(GridPoint)) continue;
			if (!IsGridPieceActive(GridPoint)) continue;
			
			Finished.Add(GridPoint);
			PreviousAttempt.Add(GridPoint, CurrentAttemptCoords);

			// Has found the target movement piece for this turn an will build a path backwards to the actor.
			if (GridPoint == TargetCoords)
			{
				// Rebuild path backwards
				FIntPoint Step = TargetCoords;
				OutPath.Add(Step);

				while (Step != StartCoords)
				{
					Step = PreviousAttempt[Step];
					OutPath.Add(Step);
				}

				Algo::Reverse(OutPath);
				return true;
			}

			// if is not target, try the next one.
			PathQueue.Enqueue(GridPoint);
		}
	}

	// If all of that fails return false, No pathway was found. 
	return false;
}


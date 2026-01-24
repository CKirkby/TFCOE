// Created by Snow Paw Games

#include "CharacterCombatData.h"

#include "AIController.h"
#include "BoardPiece.h"
#include "CombatInterface.h"
#include "HealthInterface.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "GameFramework/GameMode.h"
#include "Algo/Reverse.h"
#include "Kismet/KismetMathLibrary.h"

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
	// Testing for printing health //
	IHealthInterface* HealthInterfaceTest = Cast<IHealthInterface>(GetOwner());
	UE_LOG(LogTemp, Error, TEXT("Enemy Health this turn: %i"), HealthInterfaceTest->GetHealth());
	
	
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
		// Step 3.5: Movement
		const TArray<FCandidatePathway> PossibleLocations = GetReachableMovementPositions(CurrentTarget);
		const TArray<FIntPoint> CalculatedPathway = ChooseValidMovementPath(PossibleLocations, 0);

		StartMovementAlongGridPath(CalculatedPathway);
	}

	// Step 4: Attack
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
			// Gets the targets of these actors preferred target faction. Also checks that there are faction members of present. Otherwise, this will return empty.
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
			const int32 RandIndex = FMath::RandRange(0, PotentialTargets.Num() - 1);
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
		const int32 RandIndex = FMath::RandRange(0, PotentialTargets.Num() - 1);
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
			const int32 AttackRange = ChosenAttack->AttackRange;
			return DistToTarget > AttackRange;
		}
	
	case EAttackType::Ranged:
		{
			const int32 MinRange = ChosenAttack->AttackRange;
			const int32 MaxRange = ChosenAttack->MaxAttackRange;
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
	const int32 DistToTarget = GetGridDistanceAllDir(CurrentGridCoordinates, TargetCoordinates);

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
		const int32 RandIndex = FMath::RandRange(0, DesiredAttacks.Num() - 1);
		return DesiredAttacks[RandIndex];
	}

	// Choose any attack.
	const int32 RandIndex = FMath::RandRange(0, CachedAttacks.Num() - 1);
	return &CachedAttacks[RandIndex];
}

TArray<FCandidatePathway> UCharacterCombatData::GetReachableMovementPositions(AActor* TargetActor)
{
	TArray<FCandidatePathway> FailsafeStruct = {{CurrentGridCoordinates, 0}};
	
	if (!TargetActor || !EntityCombatConfiguration)
	{
		UE_LOG(LogTemp, Error, TEXT("Combat Data: Choose Movement Position - Reference fail"));
		return FailsafeStruct;
	}

	// Gets the interface for the target
	ICombatInterface* CombatInterfaceTarget = Cast<ICombatInterface>(TargetActor);
	if (!CombatInterfaceTarget) return FailsafeStruct;

	// Gets the coordinates of the grid the target is stood on
   	FIntPoint TargetCoordinates = CombatInterfaceTarget->GetGridCoordinates();

	// Sets an array to check whether 
	TArray<FIntPoint> CoordinatesToAttempt;
	TArray<FIntPoint> SuccessfulCandidates;

	// Populates the array with the grid pieces around the 
	GetGridAdjacentAllDir(TargetCoordinates, CoordinatesToAttempt);
	if (CoordinatesToAttempt.IsEmpty()) return FailsafeStruct;

	// Sorts through the coordinates to check to see which
	for (auto Coordinates : CoordinatesToAttempt)
	{
		if (!DoesGridCoordinatesExist(Coordinates)) continue;
		if (!IsGridPieceActive(Coordinates)) continue;

		// Culls the non-existent or occupied coordinates. 
		SuccessfulCandidates.Add(Coordinates);
	}

	if (SuccessfulCandidates.IsEmpty()) return FailsafeStruct;

	// Creates an array of those structs to prepare for storage
 	TArray<FCandidatePathway> ReachableGridPositions;
	ReachableGridPositions.Reserve(SuccessfulCandidates.Num());

	// Calculates a path to the position and how many steps it will take to reach that. Then store it in the array.
	for (FIntPoint& Candidate : SuccessfulCandidates)
	{
		int32 Distance = GetGridDistanceAllDir(CurrentGridCoordinates, Candidate);
		ReachableGridPositions.Add({Candidate, Distance});
	}

	if (ReachableGridPositions.IsEmpty()) return FailsafeStruct;

	// Sorts the structs based on how fewer steps it will take to reach, the less, the lower in the array.
	ReachableGridPositions.Sort([](const FCandidatePathway& PathA, const FCandidatePathway& PathB)
	{
		if (PathA.Distance != PathB.Distance) return PathA.Distance < PathB.Distance;
		if (PathA.GridPoint.X != PathB.GridPoint.X) return PathA.GridPoint.X < PathB.GridPoint.X;
		return PathA.GridPoint.Y < PathB.GridPoint.Y;
	});

	// Returns the struct with the least amount of steps,
	return ReachableGridPositions;
}

TArray<FIntPoint> UCharacterCombatData::ChooseValidMovementPath(const TArray<FCandidatePathway>& PossiblePositions, const int32 PathwayAttemptModifier)
{
	if (PossiblePositions.IsEmpty()) return {CurrentGridCoordinates};
	if (!EntityCombatConfiguration) return {CurrentGridCoordinates};
	
	// This is the current attempt of tries
	int32 AttemptIndex = PathwayAttemptModifier;

	// It gets the attempt of what should be the closest grid point to the target.
	int32 ClosestDistance = PossiblePositions[AttemptIndex].Distance;

	// This for loop checks the distances and if the closest distance has similar ones, It will choose one at random to be a little more dynamic
	TArray<FIntPoint> TempSimilarDistances;
	for (const FCandidatePathway Pos : PossiblePositions)
	{
		if (Pos.Distance == ClosestDistance)
		{
			TempSimilarDistances.Add(Pos.GridPoint);
		}
	}

	// Then gets a random one from that array to look into. 
	int32 RandIndex = FMath::RandRange(0, TempSimilarDistances.Num() - 1);
	const FIntPoint TargetCoordinatesForMove = PossiblePositions[RandIndex].GridPoint;
	UE_LOG(LogTemp, Error, TEXT("Movement Target: %i, %i"), TargetCoordinatesForMove.X, TargetCoordinatesForMove.Y);

	// Calculates a valid path to that point. If it cannot be reached, it isn't valid
	TArray<FIntPoint> PathToFollow;
	if (FindPathUsingAStar(CurrentGridCoordinates, TargetCoordinatesForMove, PathToFollow))
	{
		int32 MovementSpeed = EntityCombatConfiguration->CombatConfiguration.MovementRange;

		// Checks that the movement speed isn't zero or there isn't a path. 
		if (MovementSpeed <= 0 || PathToFollow.Num() <= 1)
		{
			return {CurrentGridCoordinates};;
		}

		// Configures the path so that it can only move depending on its movement speed. 
		const int32 ConfiguredMovements = FMath::Min(PathToFollow.Num(), MovementSpeed + 1);
		PathToFollow.SetNum(ConfiguredMovements);
		
		return PathToFollow;
	}

	// If this location did not have a valid path, then try again. Will iterate through the entire array of possibilities, otherwise don't move. 
	AttemptIndex++;
	if (AttemptIndex >= PossiblePositions.Num()) return {CurrentGridCoordinates};

	return ChooseValidMovementPath(PossiblePositions, AttemptIndex);
}

int32 UCharacterCombatData::CalculateMovementCost(const FIntPoint CurrentCoordinates,
                                                  const FIntPoint TargetCoordinates)
{
	// Uses the Chebyshev method to calculate action cost.

	// Gets the absolute number because I don't care the direction of movement only the distance.
	const int32 DeltaX = FMath::Abs(TargetCoordinates.X - CurrentCoordinates.X);
	const int32 DeltaY = FMath::Abs(TargetCoordinates.Y - CurrentCoordinates.Y);

	// Gets the max number between the two variables so it can calculate the max amount of cost on the axis.
	return FMath::Max(DeltaX, DeltaY);
}

FIntPoint UCharacterCombatData::CalculateTargetMovementPiece() const
{
	if (!CombatInterfacePlayer || !CombatInterfaceGamemode) return FIntPoint(1,1);

	const FIntPoint PlayerCoordinates = CombatInterfacePlayer->GetGridCoordinates();
	FIntPoint TargetCoordinates = PlayerCoordinates;

	const FIntPoint DirToTarget = FIntPoint(PlayerCoordinates.X - CurrentGridCoordinates.X, PlayerCoordinates.Y - CurrentGridCoordinates.Y);

	// Calculates which piece should be the target of this actor. 
	if (FMath::Abs(DirToTarget.X) > FMath::Abs(DirToTarget.Y))
	{
		if (DirToTarget.X < 0)
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
		if (DirToTarget.Y < 0)
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
	TMap<AActor*, int32> CachedDistances;

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
		const int32 RandIndex = FMath::RandRange(0, PotentialTargets.Num() - 1);
		return PotentialTargets[RandIndex];
	}

	UE_LOG(LogTemp, Error, TEXT("Combat Data: Get Target From Closest or Random - Complete function fail"))
	return nullptr;
}

bool UCharacterCombatData::CheckIfShouldAttack(AActor* TargetActor) const
{
	if (!TargetActor) return false;
	
	ICombatInterface* CombInterTarget = Cast<ICombatInterface>(TargetActor);
	if (!CombInterTarget) return false;
	
	FIntPoint TargetCoordinates = CombInterTarget->GetGridCoordinates();
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
	const int32 CostToMove = CalculateMovementCost(CurrentCoordinates, TargetCoordinates);

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

int32 UCharacterCombatData::GetGridDistanceAllDir(const FIntPoint& PointA, const FIntPoint& PointB) const
{
	// Uses the Chebyshev method to include Diagonals into the movement consideration
	return FMath::Max(FMath::Abs(PointA.X - PointB.X), FMath::Abs(PointA.Y - PointB.Y));
}

int32 UCharacterCombatData::GetGridDistanceCardinal(const FIntPoint& PointA, const FIntPoint& PointB) const
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
	const int32 DeltaX = FMath::Abs(PointA.X - PointB.X);
	const int32 DeltaY = FMath::Abs(PointA.Y - PointB.Y);

	return (DeltaX == 0) || (DeltaY == 0) || (DeltaX == DeltaY);
}

FVector UCharacterCombatData::GetGridPosition(const FIntPoint& Coordinates) const
{
	// Gets the owner in case fail. 
	const AActor* Owner = GetOwner();
	if (!Owner) return FVector::ZeroVector;
	
	if (CombatInterfaceGamemode)
	{
		if (AActor* GridPiece = CombatInterfaceGamemode->GetGridPieceFromCoordinates(Coordinates))
		{
			// Gets the interface for the grid piece so we can get its location.
			ICombatInterface* CombatInterfaceGrid = Cast<ICombatInterface>(GridPiece);
			if (!CombatInterfaceGrid) return Owner->GetActorLocation();

			// Returns the board piece location
			return CombatInterfaceGrid->GetBoardPieceLocation();
		}

		// If fails all that or no references. It will return the actors current location so that it will just remain still.
		UE_LOG(LogTemp, Warning, TEXT("Combat Data: Get Grid Location - No Grid Piece found."))
		return Owner->GetActorLocation();
	}

	UE_LOG(LogTemp, Warning, TEXT("Combat Data: Get Grid Location - No Gamemode interface found."))
	return Owner->GetActorLocation();
}

void UCharacterCombatData::GetGridAdjacentAllDir(const FIntPoint& OriginCoordinates,
                                                 TArray<FIntPoint>& OutNeighbors) const
{
	OutNeighbors.Reset(8);
	// Adds the four different cardinal directions, Up, down, left, right.
	OutNeighbors.Add(OriginCoordinates + FIntPoint(1, 0));
	OutNeighbors.Add(OriginCoordinates + FIntPoint(-1, 0));
	OutNeighbors.Add(OriginCoordinates + FIntPoint(0, 1));
	OutNeighbors.Add(OriginCoordinates + FIntPoint(0, -1));

	// Adds the ordinal diagonal directions. 
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

	EPieceState State = CombatInterfaceGridPiece->GetCurrentPieceState();
	
	// Checks the piece isn't occupied or non-walkable.
	return State != EPieceState::Occupied && State != EPieceState::Disabled;
}

bool UCharacterCombatData::FindPathUsingAStar(FIntPoint& StartCoords, const FIntPoint& TargetCoords,
	TArray<FIntPoint>& OutPath) const
{
	OutPath.Reset();
	
	// If the start is the target, it's the current pos already, so just return and don't move.
	if (StartCoords == TargetCoords)
	{
		OutPath.Add(StartCoords);
		return true;
	}

	// Another check if the grid position is a valid position for movement. 
	if (!DoesGridCoordinatesExist(TargetCoords) || !IsGridPieceActive(TargetCoords))
	{
		return false;
	}

	// An array of structs to store the grid positions that will be attempted to sorted through. 
	TArray<FAStarGrid> ToAttempt;

	// A map that stores the coordinates and the cost it will take to move there
	TMap<FIntPoint, int32> DistanceCost;

	// A map of previously attempted grid positions, stores the previous location to the current.
	TMap<FIntPoint, FIntPoint> PathAttempted;

	// A set of the grid position that have been finalised. 
	TSet<FIntPoint> Finished;

	
	DistanceCost.Add(StartCoords, 0); // Starting with the current pos of the character.

	FAStarGrid StartGrid;
	StartGrid.GridPoint = StartCoords;
	StartGrid.Cost = 0;
	StartGrid.Heuristic = GetGridDistanceAllDir(StartCoords, TargetCoords);
	
	ToAttempt.Add(StartGrid); // The initial attempt, starting from the pos and the cost it will take. 
	ToAttempt.Heapify(AStarHeapCheck);

	// Stores the adjoining grid points to the current grid point attempt. 
	TArray<FIntPoint> AdjacentPoints;
	AdjacentPoints.Reserve(8);

	while (ToAttempt.Num() > 0)
	{
		// Sets up the current grid position, we will attempt this iteration. 
		FAStarGrid CurrentGridPos;
		ToAttempt.HeapPop(CurrentGridPos, AStarHeapCheck);

		FIntPoint CurrentCoordinates = CurrentGridPos.GridPoint;

		// If the coordinate has already been checked, do the next. 
		if (Finished.Contains(CurrentCoordinates))
		{continue;}

		// If it has reached the target coordinates, it will add it to the path and then reverse it so it can create a stable pathway.
		if (CurrentCoordinates == TargetCoords)
		{
			FIntPoint Step = TargetCoords;
			OutPath.Add(Step);

			while (Step != StartCoords)
			{
				Step = PathAttempted[Step];
				OutPath.Add(Step);
			}

			Algo::Reverse(OutPath);
			
			return true;
		}

		Finished.Add(CurrentCoordinates);

		AdjacentPoints.Reset();
		GetGridAdjacentAllDir(CurrentCoordinates, AdjacentPoints);

		int32 CurrentCost = DistanceCost[CurrentCoordinates];

		for (FIntPoint& Point : AdjacentPoints)
		{
			if (Finished.Contains(Point)) continue;
			if (!DoesGridCoordinatesExist(Point)) continue;

			// This section is to check if there is a blocked part if the character is trying to go diagonal. 
			int32 DeltaX = Point.X - CurrentCoordinates.X;
			int32 DeltaY = Point.Y - CurrentCoordinates.Y;

			if (DeltaX != 0 && DeltaY != 0)
			{
				// Checks each side from the current path part so that it can check if either or is blocked. 
				FIntPoint SideA(CurrentCoordinates.X + DeltaX, CurrentCoordinates.Y);
				FIntPoint SideB(CurrentCoordinates.X, CurrentCoordinates.Y + DeltaY);

				// Performs the checks, will not add this to the path if it is blocked and will go around obstacles instead of cutting through them.
				if (!DoesGridCoordinatesExist(SideA) || !DoesGridCoordinatesExist(SideB)) continue;
				if (!IsGridPieceActive(SideA) || !IsGridPieceActive(SideB)) continue;
			}
			
			if (!IsGridPieceActive(Point)) continue;

			int32 PossibleCost = CurrentCost + 1;
			int32* ExistingCost = DistanceCost.Find(Point);

			if (!ExistingCost || PossibleCost < *ExistingCost)
			{
				PathAttempted.Add(Point, CurrentCoordinates);
				DistanceCost.Add(Point, PossibleCost);

				FAStarGrid Grid;
				Grid.GridPoint = Point;
				Grid.Cost = PossibleCost;
				Grid.Heuristic = GetGridDistanceAllDir(Point, TargetCoords);
				
				ToAttempt.HeapPush(Grid, AStarHeapCheck);
			}
		}
	}
	
	return false;
}

void UCharacterCombatData::OnMovementComplete(FAIRequestID RequestID, EPathFollowingResult::Type Result)
{
	// If movement for some reason fails, end the turn. 
	if (Result != EPathFollowingResult::Success)
	{
		EndThisActorTurn();
		return;
	}
	
	// Otherwise, increase the movement index and then execute it. 
	TurnPathIndex++;
	MoveToNextGridPos();
}

void UCharacterCombatData::StartMovementAlongGridPath(const TArray<FIntPoint>& Path)
{
	TurnPath = Path;
	TurnPathIndex = 1;

	MoveToNextGridPos();
}

void UCharacterCombatData::MoveToNextGridPos()
{
	if (TurnPathIndex >= TurnPath.Num())
	{
		// Attack;

		// Makes sure that when the AI finishes its movement phase, it is looking at the target.
		if (CurrentTarget)
		{
			GetOwner()->SetActorRotation(UKismetMathLibrary::FindLookAtRotation(GetOwner()->GetActorLocation(),
			CurrentTarget->GetActorLocation()));
		}

		// TODO - Move this to attack pattern instead
		
		// TESTING. If It's not the player, ends the turn. Until attack is implemented.
		if (EntityCombatConfiguration->FactionID == EFactionID::BlackLine)
		{
			EndThisActorTurn();
			return;
		}
		
		return;
	}

	// Gets the next grid pos that is lined up from the array of 
	FIntPoint NextGridPos = TurnPath[TurnPathIndex];
	FVector NextMovementLocation = GetGridPosition(NextGridPos);

	// Gets the AI Controller needed for movement.
	if (ACharacter* Owner = Cast<ACharacter>(GetOwner()))
	{
		if (AAIController* AIController = Cast<AAIController>(Owner->GetController()))
		{
			// Binds on movement completed to trigger on movement completed to either end movement or move to the next tile.
			AIController->ReceiveMoveCompleted.RemoveDynamic(this, &UCharacterCombatData::OnMovementComplete);
			AIController->ReceiveMoveCompleted.AddDynamic(this, &UCharacterCombatData::OnMovementComplete);

			// Triggers the movement code. 
			AIController->MoveToLocation(NextMovementLocation, 5.0f, false);
		}
	}
}


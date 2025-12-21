// Created by Snow Paw Games

#include "CombatManager.h"
#include "CombatInterface.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"


UCombatManager::UCombatManager()
{ 
	PrimaryComponentTick.bCanEverTick = false;
}

void UCombatManager::BeginPlay()
{
	Super::BeginPlay();
}

void UCombatManager::SetCombatState(const int CombatState)
{
	// Tracks the current state of the game
	CurrentCombatState = CombatState;

	// 0 -> Disengaged
	// 1 -> Engaged
	switch (CombatState)
	{
	case 0:
		// Notifies the player character of combat end
		NotifyPlayerOfCombatStatus(0);
		
		// Ends the combat resetting the values
		EndCombat();
		
		break;
		
	case 1:
		// Notifies the player character of combat start
		NotifyPlayerOfCombatStatus(1);
		
		// Starts the combat and sets the turn order to the first. 
		SetTurnOrder(1);
		
		break;
	default:
		NotifyPlayerOfCombatStatus(0);
	}
}

void UCombatManager::EndCurrentTurn()
{
	// Gets the current turn order and increments it, moving to the next turn.
	int TurnIndex = CurrentTurnIndex;
	TurnIndex++;

	// If the turn order has reached the end, loops back to the initial turn order
	if (TurnIndex > 3)
	{
		TurnIndex = 1;
		SetTurnOrder(TurnIndex);
	}

	// Updates the current turn order. 
	SetTurnOrder(TurnIndex);
}

void UCombatManager::SetTurnOrder(const int NewTurnOrder)
{
	switch (NewTurnOrder)
	{
		case 1:
			// Initial Turn order.
			CurrentTurnIndex = 1;
			CurrentTurnOrder = TurnOrder.FindRef(1);
		break;

		case 2:
			// Second Turn order
			CurrentTurnIndex = 2;
			CurrentTurnOrder = TurnOrder.FindRef(2);
		break;

		case 3:
			// Third turn order
			CurrentTurnIndex = 3;
			CurrentTurnOrder = TurnOrder.FindRef(3);
		break;
	
		default:
			// Defaults to the initial turn order if for any reason fails. 
			CurrentTurnIndex = 1;
			CurrentTurnOrder = TurnOrder.FindRef(1);
		;
	}

	// After updating the turn order, will execute the relevant functionality. 
	ExecuteTurnFunctionality(CurrentTurnOrder);
}

void UCombatManager::ExecuteTurnFunctionality(ETurnOrder NewTurn)
{
	switch (NewTurn)
	{
	case Player:
		
		//Tells the player to begin their turn phase.
		if (ICombatInterface* CombatInterfacePlayer = Cast<ICombatInterface>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
		{
			//CombatInterfacePlayer->BeginTurnPhase();
		}
		
		break;
		
	case Enemy:
		
		ExecuteEnemyTurn();
		
		break;
		
	case Companion:
		break;
		
	case None:
		break;
	}
}

void UCombatManager::EndCombat()
{
	CurrentTurnIndex = -1;
	CurrentTurnOrder = None;
}

void UCombatManager::SetActiveCombatants(const TArray<AActor*>& NewCombatants)
{
	ActiveCombatantRoster = NewCombatants;
}

void UCombatManager::AddActiveCombatant(AActor* NewCombatant)
{
	if (!NewCombatant) return;
	ActiveCombatantRoster.Add(NewCombatant);
}

void UCombatManager::ClearActiveCombatants()
{
	ActiveCombatantRoster.Empty();
}

// Simply adds the player party to the active combatant roster.
void UCombatManager::AddPlayerPartyToActiveCombatants()
{
	if (PlayerPartyRoster.IsEmpty()) return;

	for (const auto Actor : PlayerPartyRoster)
	{
		AddActiveCombatant(Actor);
	}
}

void UCombatManager::QueueFactionGroupsForTurn()
{
	for (auto Combatant : ActiveCombatantRoster)
	{
		ICombatInterface* CombatInterface = Cast<ICombatInterface>(Combatant);
		if (!CombatInterface) continue;

		EFactionID FactionID = CombatInterface->GetActorFactionID();
		EEnemyTier EnemyRank = CombatInterface->GetActorFactionRank();

		FFactionTierContainer& Container = FactionTurnGroups.FindOrAdd(FactionID);

		switch (EnemyRank)
		{
		case EEnemyTier::Faction_Leader:
			Container.FactionLeader.Add(Combatant);
			break;
		case EEnemyTier::Lieutenant:
			Container.Lieutenant.Add(Combatant);
			break;
		case EEnemyTier::Elite:
			Container.Elite.Add(Combatant);
			break;
		case EEnemyTier::Grunt:
			Container.Grunt.Add(Combatant);
			break;
		}
	}
}

TArray<EFactionID> UCombatManager::OrderFactionsForTurn()
{
	TArray<EFactionID> FactionsToSort;

	// Checks to see both what factions are available this combat and check the faction has actors stored
	for (const auto& Pair : FactionTurnGroups)
	{
		EFactionID FactionID = Pair.Key;

		// This makes sure the faction actually has a member inside and not empty.
		const auto& [FactionLeader, Lieutenant, Elite, Grunt] = Pair.Value;
		if (FactionLeader.Num() + Lieutenant.Num() + Elite.Num() + Grunt.Num() > 0)
		{
			FactionsToSort.Add(FactionID);
		}
	}

	// This creates a priority system to check which faction has priority, right now they are simply ordered on what I believe is the best lore wise.
	// This may be changed to be more attribute dependent in future.
	TMap<EFactionID, int> PriorityMap = {
		{EFactionID::Forlorn, FactionTurnPriority.Contains(EFactionID::Forlorn) ? FactionTurnPriority.FindRef(EFactionID::Forlorn) : 1},
		{EFactionID::BlackLine, FactionTurnPriority.Contains(EFactionID::Forlorn) ? FactionTurnPriority.FindRef(EFactionID::BlackLine) : 3},
		{EFactionID::EmpireOfEos, FactionTurnPriority.Contains(EFactionID::Forlorn) ? FactionTurnPriority.FindRef(EFactionID::EmpireOfEos) : 4},
		{EFactionID::Creature, FactionTurnPriority.Contains(EFactionID::Forlorn) ? FactionTurnPriority.FindRef(EFactionID::Creature) : 2}
	};

	// Sorts the entries based on the above criteria. Then returns the ordered factions to be used in the turn route. 
	FactionsToSort.Sort([this, &PriorityMap](const EFactionID& FactionA, const EFactionID& FactionB)
	{
		const int PriorityA = PriorityMap.Contains(FactionA) ? PriorityMap[FactionA] : 999;
		const int PriorityB = PriorityMap.Contains(FactionB) ? PriorityMap[FactionB] : 999;
		return PriorityA < PriorityB;
	});

	return FactionsToSort;
}

void UCombatManager::ExecuteEnemyTurn()
{
	// Clears the turn information from last turn.
	FactionTurnGroups.Empty();

	// Queues the new information for this turn. Populates the faction groups with each respective combatants. 
	QueueFactionGroupsForTurn();

	// Sorts which factions should be going before the others. 
	FactionTurnOrder = OrderFactionsForTurn();
	
	// Sets up the iteration. Makes sure that the rank turn is set to which I desire to go first. I want the leaders to go first. 
	CurrentFactionTurnIndex = 0;
	CurrentFactionRankTurn = EEnemyTier::Faction_Leader;

	// Iterates through the factions and then the ranks and activates the enemies turns. 
	ExecuteIndividualEnemyTurn();
}

void UCombatManager::ExecuteIndividualEnemyTurn()
          {
	// If no more enemies in the faction list then enemy turn will be complete.
	if (CurrentFactionTurnIndex >= FactionTurnOrder.Num())
	{
		EndCurrentTurn();
		return;
	}

	EFactionID CurrentFactionTurn = FactionTurnOrder[CurrentFactionTurnIndex];
	FFactionTierContainer& FactionContainer = FactionTurnGroups.FindOrAdd(CurrentFactionTurn);

	// This is to get the current factions rank list of enemies. 
	TArray<AActor*>* PoppedFactionRanksToAct = nullptr;

	// Gets the actors of the specific faction rank to act depending on which is next
	switch (CurrentFactionRankTurn)
	{
	case EEnemyTier::Grunt:
		PoppedFactionRanksToAct = &FactionContainer.Grunt;
		break;
	case EEnemyTier::Elite:
		PoppedFactionRanksToAct = &FactionContainer.Elite;
		break;
	case EEnemyTier::Lieutenant:
		PoppedFactionRanksToAct = &FactionContainer.Lieutenant;
		break;
	case EEnemyTier::Faction_Leader:
		PoppedFactionRanksToAct = &FactionContainer.FactionLeader;
		break;
	}

	// Checks if it has finished the current ranks enemies before moving onto the next
	if (!PoppedFactionRanksToAct || PoppedFactionRanksToAct->Num() == 0)
	{
		// Checks if it has finished this faction as a whole.
		if (CurrentFactionRankTurn == EEnemyTier::Grunt)
		{
			// If the faction has finished, increase the turn index and reset the rank order
			CurrentFactionTurnIndex++;
			CurrentFactionRankTurn = EEnemyTier::Faction_Leader;
		}
		else
		{
			// Increase the faction rank to iterate through. Leader -> Lieutenant -> Elite etc...
			CurrentFactionRankTurn = static_cast<EEnemyTier>(StaticCast<uint8>(CurrentFactionRankTurn) + 1);
		}

		ExecuteIndividualEnemyTurn();
		return;
	}

	AActor* NextEnemy = PoppedFactionRanksToAct->Pop();
	if (!NextEnemy)
	{
		ExecuteIndividualEnemyTurn();
		return;
	}

	if (ICombatInterface* CombatInterfaceEnemy = Cast<ICombatInterface>(NextEnemy))
	{
		CombatInterfaceEnemy->BeginTurnPhase();
	}
	
}

// Function to send an interface message to the player stating what the combat status is.
void UCombatManager::NotifyPlayerOfCombatStatus(const int CombatState) const
{
	if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
	{
		CombatInterface->NotifyCombatStatus(CombatState);
	}
}
// Created by Snow Paw Games

#include "CombatManager.h"


UCombatManager::UCombatManager()
{ 
	PrimaryComponentTick.bCanEverTick = false;
}

void UCombatManager::BeginPlay()
{
	Super::BeginPlay();
}

void UCombatManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//Tick turned off
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
		OnCombatEnd.Broadcast();
		EndCombat();
		
		break;
	case 1:
		OnCombatBegin.Broadcast();
		
		// Starts the combat and sets the turn order to the first. 
		SetTurnOrder(1);
		
		break;
	default:
		OnCombatEnd.Broadcast();
	}
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

void UCombatManager::EndCombat()
{
	CurrentTurnIndex = -1;
	CurrentTurnOrder = None;
}

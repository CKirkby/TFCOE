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
		break;
	case 1:
		OnCombatBegin.Broadcast();
		break;
	default:
		OnCombatEnd.Broadcast();
	}
}

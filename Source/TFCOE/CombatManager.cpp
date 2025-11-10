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

void UCombatManager::SetCombatState(const ECombatState NewState)
{
	switch (NewState)
	{
	case Engaged:
		CurrentCombatState = Engaged;
		OnCombatBegin.Broadcast();
		break;
	case Disengaged:
		CurrentCombatState = Disengaged;
		OnCombatEnd.Broadcast();
		break;
	}
}


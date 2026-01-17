// Created by Snow Paw Games

#include "HealthComponent.h"

#include "CombatInterface.h"

UHealthComponent::UHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UHealthComponent::InitialiseHealth(const int NewHealth)
{
	MaxHealth = NewHealth;
	Health = MaxHealth;

	// Failsafe
	if (Health <= 0)
	{
		Health = 1;
	}
}


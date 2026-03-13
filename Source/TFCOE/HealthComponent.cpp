// Created by Snow Paw Games

#include "HealthComponent.h"
#include "Kismet/KismetMathLibrary.h"

UHealthComponent::UHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UHealthComponent::LerpActorLocation(AActor* TargetActor, const FVector PointA, const FVector PointB, float Alpha)
{
	if (!TargetActor)
	{
		TargetActor = GetOwner();
		if (!TargetActor) return;
	}
	
	TargetActor->SetActorLocation(UKismetMathLibrary::VLerp(PointA, PointB, Alpha));
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

void UHealthComponent::TakeDamage(const int Damage)
{
	// Checks to see if the target is invulnerable, if so just return
	if (IsInvulnerable) return;
	
	Health -= Damage;
	
	OnTakeDamage.Broadcast();
}

void UHealthComponent::DeathCheck() const
{
	if (Health <= 0)
	{
		// Testing for now
		GetOwner()->Destroy();
	}
}

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
	Health -= Damage;
	
	OnTakeDamage.Broadcast();
}

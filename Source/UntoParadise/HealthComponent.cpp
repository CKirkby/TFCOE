// Created by Kairo Forge

#include "HealthComponent.h"

#include "HealthInterface.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/GameModeBase.h"
#include "Kismet/KismetMathLibrary.h"

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

void UHealthComponent::TakeDamage(const int Damage)
{
	// Checks to see if the target is invulnerable, if so just return
	if (IsInvulnerable) return;
	
	Health -= Damage;
	
	// Checks to make sure the owner is not defeated. 
	DeathCheck();
	
	// Plays camera shake when the unit is hit.
	PlayOnHitCameraShake();
	
	OnTakeDamage.Broadcast();
}

void UHealthComponent::DeathCheck() const
{
	if (Health <= 0)
	{
		IHealthInterface* HI_Gamemode = Cast<IHealthInterface>(UGameplayStatics::GetGameMode(GetWorld()));
		if (!HI_Gamemode) return;
		
		if (AActor* Owner = GetOwner())
		{
			HI_Gamemode->NotifyUnitDefeated(Owner);
			GetOwner()->SetActorHiddenInGame(true);
		}
	}
}

void UHealthComponent::PlayOnHitCameraShake() const
{
	if (OnHitCameraShake.IsNull()) return;
	
	FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
	
	Streamable.RequestAsyncLoad(OnHitCameraShake.ToSoftObjectPath(), FStreamableDelegate::CreateUObject(this, &UHealthComponent::OnHitShakeAsyncLoad));
}

void UHealthComponent::OnHitShakeAsyncLoad() const
{
	UClass* LoadedClass = OnHitCameraShake.Get();
	if (!LoadedClass) return;
	
	if (APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0))
	{
		PlayerController->ClientStartCameraShake(LoadedClass);
	}
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

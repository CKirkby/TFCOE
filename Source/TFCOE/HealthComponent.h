// Created by Snow Paw Games

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TFCOE_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UHealthComponent();
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTakeDamage);
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnTakeDamage OnTakeDamage;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health")
	int Health = 5;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health")
	int MaxHealth = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	bool IsInvulnerable = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
	UMaterialInstance* OnDamageMaterial = nullptr;
	
	UFUNCTION(BlueprintCallable, Category = "Damage")
	void LerpActorLocation(AActor* TargetActor, FVector PointA, FVector PointB, float Alpha);

public:

	void InitialiseHealth(const int NewHealth);
	void TakeDamage(const int Damage);
	void DeathCheck() const;

	// Getter and Setter //

	void SetHealth(const int NewHealth)
	{
		Health = NewHealth;
	}

	void AddHealth(const int NewHealth)
	{
		Health += NewHealth;
	}

	int GetHealth() const
	{
		return Health;
	}

	bool IsDead() const
	{
		return Health >= 0;
	}

	void ResetHealth()
	{
		Health = MaxHealth;
	}
};

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

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health")
	int Health = 5;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health")
	int MaxHealth = 5;

public:

	void InitialiseHealth(const int NewHealth);

	// Getter and Setter //

	void SetHealth(const int NewHealth)
	{
		Health = NewHealth;
	}

	void TakeDamage(const int Damage)
	{
		Health -= Damage;
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

// Created by Snow Paw Games

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "HealthInterface.generated.h"

UINTERFACE(Blueprintable)
class TFCOE_API UHealthInterface : public UInterface
{
	GENERATED_BODY()
};

class TFCOE_API IHealthInterface
{
	GENERATED_BODY()

	public:

	virtual void SetHealth(int NewHealth) = 0;
	virtual void TakeDamage(int IncomingDamage) = 0;
	virtual void AddHealth(int IncomingHealth) = 0;
	virtual int GetHealth() = 0;
	virtual bool IsDead() = 0;
};
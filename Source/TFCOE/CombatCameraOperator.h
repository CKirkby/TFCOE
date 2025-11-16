// Created by Snow Paw Games

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CombatCameraOperator.generated.h"

UCLASS()
class TFCOE_API ACombatCameraOperator : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACombatCameraOperator();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY()
	UCharacterMovementComponent* MovementComponent = nullptr;

public:
	void InitialiseMovementComponent();
	
};

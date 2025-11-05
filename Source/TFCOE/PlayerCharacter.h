// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperZDCharacter.h"
#include "EnhancedInputSubsystems.h"
#include "PlayerCharacter.generated.h"

/**
 * 
 */
UCLASS()
class TFCOE_API APlayerCharacter : public APaperZDCharacter
{
	GENERATED_BODY()

	protected:

	// Input Mapping & Actions // 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input|MappingContext")
	UInputMappingContext* MappingContext = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input|Actions")
	UInputAction* MoveAction = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input|Actions")
	UInputAction* SprintAction = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input|Actions")
	UInputAction* InteractAction = nullptr;

	// Settings
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Movement")
	float WalkSpeed = 400.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Movement")
	float SprintSpeed = 700.0f;

	// Components
	UPROPERTY()
	UCharacterMovementComponent* MovementComponent = nullptr;
	
	// Functions
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	void InitialiseMovementComponent();
	
	// Input Functions
	void MoveTrigger(const FInputActionValue& Value);
	void SprintTrigger();
	void SprintEnd();
	void InteractTrigger();
};

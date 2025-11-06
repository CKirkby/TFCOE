// Created by Snow Paw Games

#pragma once

#include "CoreMinimal.h"
#include "PaperZDCharacter.h"
#include "EnhancedInputSubsystems.h"
#include "PlayerCharacter.generated.h"

class UCharacter_Inventory;
/**
 * 
 */
UCLASS()
class TFCOE_API APlayerCharacter : public APaperZDCharacter
{
	GENERATED_BODY()

	APlayerCharacter();

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
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCharacter_Inventory* CharacterInventory = nullptr;
	
	// Functions
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	void InitialiseMovementComponent();
	
	// Input Functions
	void MoveTrigger(const FInputActionValue& Value);
	void SprintTrigger();
	void SprintEnd();
	void InteractTrigger();

	// Getter & Setter
	UCharacter_Inventory* GetInventory() const
	{
		return CharacterInventory;
	}
};

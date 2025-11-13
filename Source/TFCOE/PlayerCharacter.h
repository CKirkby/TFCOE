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

public:

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBoardPieceClicked, AActor*, BoardPiece);
	UPROPERTY(BlueprintAssignable, Category="Events")
	FOnBoardPieceClicked OnBoardPieceClicked;

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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input|Actions")
	UInputAction* CombatClickAction = nullptr;

	// Settings
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Movement")
	bool MovementEnabled = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Movement")
	bool CameraMovementEnabled = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Movement")
	float WalkSpeed = 400.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Movement")
	float SprintSpeed = 700.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Combat")
	TSoftClassPtr<AActor> CombatPlayer;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Combat")
	bool CombatModeActivated = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Settings|Combat")
	AActor* AIPlayerDummy = nullptr;

	// Components
	UPROPERTY()
	UCharacterMovementComponent* MovementComponent = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCharacter_Inventory* CharacterInventory = nullptr;
	
	// Functions
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	void InitialiseMovementComponent();

	UFUNCTION(BlueprintCallable, Category = "Settings")
	void UpdatePlayerCombatState(bool CombatEnabled);
	
	// Input Functions
	void MoveTrigger(const FInputActionValue& Value);
	void ResetMovement();
	void SprintTrigger();
	void SprintEnd();
	void InteractTrigger();

	void CombatClickTrigger();

	// Getter & Setter
	UCharacter_Inventory* GetInventory() const
	{
		return CharacterInventory;
	}

	UFUNCTION(BlueprintCallable, Category = "Settings")
	void SetLocomotion(const bool EntityMovement, const bool CameraMovement)
	{
		MovementEnabled = EntityMovement;
		CameraMovementEnabled = CameraMovement;
	}

	UFUNCTION(BlueprintCallable, Category = "Settings")
	AActor* GetPlayerAIDummy() const
	{
		return AIPlayerDummy;
	}
};

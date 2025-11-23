// Created by Snow Paw Games

#pragma once

#include "CoreMinimal.h"
#include "PaperZDCharacter.h"
#include "EnhancedInputSubsystems.h"
#include "PlayerCharacter.generated.h"

class ACombatCameraOperator;
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

	// Combat References 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Combat")
	TSoftClassPtr<AActor> AIPlayerDummyClass = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Combat")
	TSoftClassPtr<ACombatCameraOperator> CameraOperatorClass = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Combat")
	bool CombatModeActivated = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Settings|Combat")
	AActor* AIPlayerDummy = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Settings|Combat")
	ACombatCameraOperator* CameraOperator = nullptr;

	// Components
	UPROPERTY()
	UCharacterMovementComponent* MovementComponent = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCharacter_Inventory* CharacterInventory = nullptr;
	UPROPERTY()
	APlayerController* PlayerController = nullptr;
	
	// Functions
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	
	void InitialiseMovementComponent();

	// Combat Functions
	void EnterCombatMode();
	void ExitCombatMode();
	void AsyncLoadDummy();
	void SetMainActorHidden(bool SetHidden);
	void DestroyDummy();
	void SpawnAndSetCameraOperator();
	void ReturnAndDestroyCameraOperator();

	UFUNCTION(BlueprintCallable, Category = "Settings")
	void UpdatePlayerCombatState(bool CombatEnabled);
	
	// Input Functions
	void MoveTrigger(const FInputActionValue& Value);
	UFUNCTION(BlueprintCallable, Category = "Settings")
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

	UFUNCTION(BlueprintCallable, Category = "Player")
	void SetLocomotion(const bool EntityMovement, const bool CameraMovement)
	{
		MovementEnabled = EntityMovement;
		CameraMovementEnabled = CameraMovement;
	}

	UFUNCTION(BlueprintCallable, Category = "Player")
	AActor* GetPlayerAI_Dummy() const
	{
		return AIPlayerDummy;
	}

	UFUNCTION(BlueprintCallable, Category = "Player")
	FVector GetAI_DummyLocation() const
	{
		if (AIPlayerDummy)
		{
			return AIPlayerDummy->GetActorLocation();
		}
		return FVector::ZeroVector;
	}

	UFUNCTION(BlueprintCallable, Category = "Player")
	ACombatCameraOperator* GetCameraOperator() const
	{
		return CameraOperator;
	}
};

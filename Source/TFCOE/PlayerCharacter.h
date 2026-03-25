// Created by Kairo Forge

#pragma once

#include "CoreMinimal.h"
#include "AI_PlayerCombatant.h"
#include "PaperZDCharacter.h"
#include "EnhancedInputSubsystems.h"
#include "EntityID.h"
#include "CombatInterface.h"
#include "PlayerCharacter.generated.h"

class UAttackController;
class AAI_PlayerCombatant;
class ACombatCameraOperator;
class UCharacter_Inventory;

UENUM(BlueprintType)
enum class EPlayerTurnState : uint8
{
	Neutral UMETA(DisplayName="Neutral"),
	MovementMode UMETA(DisplayName="MovementMode"),
	CombatMode UMETA(DisplayName="CombatMode")
};


/**
 * 
 */
UCLASS()
class TFCOE_API APlayerCharacter : public APaperZDCharacter, public ICombatInterface
{
private:
	GENERATED_BODY()

private:

	APlayerCharacter();

public:
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Events")
	void OnSuccessfulMovementTriggered();
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Events")
	void OnNewCameraTargetSelected(AActor* Target);
	
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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input|Actions")
	UInputAction* EndTurnAction = nullptr;

	// Settings
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Movement")
	bool MovementEnabled = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Movement")
	bool CameraMovementEnabled = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Movement")
	float WalkSpeed = 400.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Movement")
	float SprintSpeed = 700.0f;
	
	int CurrentTimePoints = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Configuration")
	EEntityID EntityID = EEntityID::Player;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Configuration")
	EFactionID FactionID = EFactionID::Player;

	// Combat References 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Combat")
	TSoftClassPtr<AActor> AIPlayerDummyClass = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Combat")
	TSoftClassPtr<ACombatCameraOperator> CameraOperatorClass = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Combat")
	bool CombatModeActivated = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Settings|Combat")
	AAI_PlayerCombatant* PlayerCombatant = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Settings|Combat")
	ACombatCameraOperator* CameraOperator = nullptr;
	
	// Hover References
	UPROPERTY() AActor* CurrentGridPieceHovered = nullptr;
	UPROPERTY() AActor* CurrentTargetHovered = nullptr;
	
	// Combat functionality
	bool InitialTurn = true;
	bool HoverModeActive = false;
	FTimerHandle HoverModeHandle;
	
	EPlayerTurnState CurrentPlayerTurnState = EPlayerTurnState::Neutral;
	EPlayerTurnState LastTurnStateUsed;

	// Components
	UPROPERTY()
	UCharacterMovementComponent* MovementComponent = nullptr;
	UPROPERTY()
	APlayerController* PlayerController = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCharacter_Inventory* CharacterInventory = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UAttackController* AttackController = nullptr;
	
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
	bool CheckIsPlayersTurn() const;
	bool CheckGridSlotAvailable(AActor* BoardPieceActor);
	void OnBoardPieceClicked(AActor* BoardPiece);
	void OnTargetCombatantClicked(const AActor* Target);
	bool CheckCanAffordAttack(const FAttackConfiguration* ChosenAttack) const;
	
	bool CheckGridPieceActive(AActor* TargetPiece);
	FAttackConfiguration* GetAttackConfig(FName AttackID) const;
	
	// Hover Functionality
	void EnterHoverMode();
	void CheckHover_Movement();
	void CheckHover_Enemy();
	void ExitHoverMode();
	void ClearCachedHover();
	void NotifyGridOnHoverEnd() const;
	void NotifyTargetOnHoverEnd() const;

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
	void EndTurnTrigger();
	
	int32 GetGridDistanceAllDir(const FIntPoint& PointA, const FIntPoint& PointB);
	bool IsTargetWithinRange(AActor* Target);
	
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
	AActor* GetPlayerCombatant_Vague() const
	{
		if (PlayerCombatant)
		{
			AActor* CombatantActor = PlayerCombatant;
			return  CombatantActor;
		}
		
		return nullptr;
	}
	
	UFUNCTION()
	AAI_PlayerCombatant* GetPlayerCombatant_Ref() const
	{
		return PlayerCombatant ? PlayerCombatant : nullptr;
	}

	UFUNCTION(BlueprintCallable, Category = "Player")
	FVector GetPlayerCombatantLocation() const
	{
		if (PlayerCombatant)
		{
			return PlayerCombatant->GetActorLocation();
		}
		
		return FVector::ZeroVector;
	}

	UFUNCTION(BlueprintCallable, Category = "Player")
	ACombatCameraOperator* GetCameraOperator() const
	{
		return CameraOperator;
	}

	UFUNCTION(blueprintCallable, Category = "Player")
	bool GetCombatModeActivated() const
	{
		return CombatModeActivated;
	}
	
	UFUNCTION(BlueprintCallable, Category = "Player")
	void SetCombatTurnMode(EPlayerTurnState NewTurnState);
	

	// Interface Implementation
	virtual void NotifyCombatStatus(int CombatState) override;
	virtual AActor* GetPlayerCombatant() override {return GetPlayerCombatant_Vague();}
	virtual FVector GetCombatPlayerLocation() override {return PlayerCombatant->GetActorLocation();}
	virtual void MoveAI_Character(FVector Location) override;
	virtual FIntPoint GetGridCoordinates() override;
	virtual EFactionID GetActorFactionID() override {return FactionID;}
	virtual EEntityID GetActorEntityID() override {return EEntityID::Player;}
	virtual void BeginTurnPhase() override;
	virtual void SetAttackerReference(AActor* AttackerReference) override;
	virtual int GetTimePoints() override;
	virtual void SetPlayerHoverMovementModeActive(bool IsActivate) override;
	virtual void NotifyNewCameraFocus(AActor* Target) override;
	virtual void NotifyTimePointsExpended(const int32 Amount) override {PlayerCombatant->NotifyTimePointsExpended(Amount);}

	// Unneeded Interface Implementations
	// Player
	virtual void NotifyEndIndividualTurn() override {}
	virtual void NotifyMovementRequirementsMet(AActor* BoardPiece) override {}
	virtual void SetCombatantCoordinates(FIntPoint Coordinates) override {}

	// Gamemode
	virtual void NotifyEndTurnTriggered() override {}
	virtual ETurnOrder GetCurrentTurnOrder() override {return ETurnOrder();}
	virtual AActor* GetGridPieceFromCoordinates(FIntPoint Coordinates) override {return nullptr;}
	virtual TArray<AActor*> GetActiveCombatantRoster() override {return TArray<AActor*>();}
	virtual EEnemyTier GetActorFactionRank() override {return EEnemyTier::Grunt;}

	// Board Piece
	virtual void NotifyPieceClicked() override {}
	virtual FVector GetBoardPieceLocation() override {return FVector::ZeroVector;}
	virtual EPieceState GetCurrentPieceState() override {return EPieceState();}
	virtual bool DoesGridContainCoordinate(FIntPoint CoordsToCheck) override {return false;}
};

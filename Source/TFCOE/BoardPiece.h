// Created by Snow Paw Games

#pragma once

#include "CombatInterface.h"

#include "CoreMinimal.h"
#include "BoardControllerInterface.h"
#include "CombatManager.h"
#include "GameFramework/Actor.h"
#include "BoardPiece.generated.h"

class UWidgetComponent;
class UBoxComponent;

UENUM(BlueprintType)
enum EPieceState
{
	Enabled UMETA(DisplayName = "Enabled"),
	Disabled UMETA(DisplayName = "Disabled"),
	Occupied UMETA(DisplayName = "Occupied"),
};

UCLASS()
class TFCOE_API ABoardPiece : public AActor, public ICombatInterface, public IBoardControllerInterface
{
	
private:
	GENERATED_BODY()

public:	
	ABoardPiece();

protected:

	// Components
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* StaticMesh = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UBoxComponent* BoxCollision = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	USceneComponent* CharacterPosition = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UWidgetComponent* TargetIndicatorWidget = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	FIntPoint GridPosition;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	TEnumAsByte<EPieceState> CurrentPieceState = Enabled;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Materials")
	UMaterialInterface* OriginalMaterial = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Materials")
	UMaterialInterface* MovementClickMaterial = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Materials")
	UMaterialInterface* ReachableMovementMaterial = nullptr;

	UPROPERTY()
	AActor* CurrentOccupier = nullptr;
	
	virtual void BeginPlay() override;

public:
	
	UFUNCTION(BlueprintCallable, Category = "Board Piece")
	void TriggerOverlapFunctionality(AActor* OtherActor);
	UFUNCTION(BlueprintCallable, Category = "Board Piece")
	void TriggerOverlapEndFunctionality(AActor* OtherActor);

	void ResetMaterial();
	
	// Getter and Setter

	UFUNCTION(BlueprintCallable, Category = "Board Piece")
	void SetPieceState(const EPieceState NewPieceState)
	{
		CurrentPieceState = NewPieceState;
	}

	UFUNCTION(BlueprintCallable, Category = "Board Piece")
	void SetGridCoordinates(FIntPoint NewCoordinates);

	UFUNCTION(BlueprintCallable, Category = "Board Piece")
	EPieceState GetPieceState() const
	{
		return CurrentPieceState;
	}

	UFUNCTION(BlueprintCallable, Category = "Board Piece")
	AActor* GetCurrentOccupier() const
	{
		return CurrentOccupier;
	}

	UFUNCTION(BlueprintCallable, Category = "Board Piece")
	void SetCurrentOccupier(AActor* NewOccupier)
	{
		if (NewOccupier)
		{
			CurrentOccupier = NewOccupier;
		}
	}

	UFUNCTION(BlueprintCallable, Category = "Board Piece")
	void ClearCurrentOccupier()
	{
		CurrentOccupier = nullptr;
	}

	UFUNCTION(BlueprintCallable, Category = "Board Piece")
	FIntPoint GetGridPosition() const
	{
		return GridPosition;
	}

	UFUNCTION(BlueprintCallable, Category = "Board Piece")
	void SetBoxCollider(UBoxComponent* NewBox)
	{
		if (NewBox)
		{
			BoxCollision = NewBox;
		}
	}

	
	
	
	
	
	// Interface Implementations
	virtual void NotifyPieceClicked() override;
	virtual FVector GetBoardPieceLocation() override;
	virtual EPieceState GetCurrentPieceState() override {return CurrentPieceState;}
	virtual FIntPoint GetGridCoordinates() override {return GridPosition;};
	
	virtual void NotifyBoardPieceHighlight(EHighlightType Type) override;
	virtual void NotifyBoardPieceOnHover() override;
	virtual void NotifyBoardPieceOnHoverEnd() override;

	// Unneeded Interface Implementations
	// Player
	virtual void NotifyEndIndividualTurn() override {}
	virtual FVector GetCombatPlayerLocation() override {return FVector::ZeroVector;}
	virtual AActor* GetPlayerCombatant() override {return nullptr;}
	virtual void NotifyMovementRequirementsMet(AActor* BoardPiece) override {};
	virtual void NotifyCombatStatus(int CombatState) override {}
	virtual int GetTimePoints() override {return 0;}
	virtual void SetCombatantCoordinates(FIntPoint Coordinates) override {}
	virtual void SetPlayerHoverMovementModeActive(bool IsActivate) override {}
	virtual void NotifyNewCameraFocus(AActor* Target) override {}
	virtual void NotifyTimePointsExpended(const int32 Amount) override {}

	// Gamemode
	virtual void NotifyEndTurnTriggered() override {}
	virtual ETurnOrder GetCurrentTurnOrder() override {return ETurnOrder();}
	virtual void MoveAI_Character(FVector Location) override {}
	virtual AActor* GetGridPieceFromCoordinates(FIntPoint Coordinates) override {return nullptr;}
	virtual void BeginTurnPhase() override {}
	virtual TArray<AActor*> GetActiveCombatantRoster() override {return TArray<AActor*>();}
	virtual EFactionID GetActorFactionID() override {return EFactionID::None;}
	virtual EEnemyTier GetActorFactionRank() override { return EEnemyTier::Grunt;}
	virtual EEntityID GetActorEntityID() override {return EEntityID::Uninitialized_ID;}
	virtual bool DoesGridContainCoordinate(FIntPoint CoordsToCheck) override {return false;}
	virtual void SetAttackerReference(AActor* AttackerReference) override {}
	
	// Board Controller 
	virtual void SetReachableMovementPositionsVisible(bool Active) override {}
	virtual void SetAttackPositionsVisible(TArray<FIntPoint> Positions) override {}
	virtual TArray<FIntPoint> GetAllBoardPieces() override {return TArray<FIntPoint>();}
	virtual void ResetHighlightedPieces() override {}
	virtual void NotifyTargetOnHover() override {}
	virtual void NotifyTargetOnHoverEnd() override {}
};

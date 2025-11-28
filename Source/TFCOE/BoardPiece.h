// Created by Snow Paw Games

#pragma once

#include "CombatInterface.h"

#include "CoreMinimal.h"
#include "CombatManager.h"
#include "GameFramework/Actor.h"
#include "BoardPiece.generated.h"

class UBoxComponent;

UENUM(BlueprintType)
enum EPieceState
{
	Enabled UMETA(DisplayName = "Enabled"),
	Disabled UMETA(DisplayName = "Disabled"),
	Occupied UMETA(DisplayName = "Occupied"),
};

UCLASS()
class TFCOE_API ABoardPiece : public AActor, public ICombatInterface
{
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
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	FVector2D GridPosition;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	TEnumAsByte<EPieceState> CurrentPieceState = Enabled;

	UPROPERTY()
	AActor* CurrentOccupier = nullptr;
	
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;

public:

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	// Getter and Setter

	UFUNCTION(BlueprintCallable, Category = "Board Piece")
	void SetPieceState(const EPieceState NewPieceState)
	{
		CurrentPieceState = NewPieceState;
	}

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
	FVector2D GetGridPosition() const
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
	virtual void NotifyPieceClicked() override {}
	virtual FVector GetBoardPieceLocation() override;
	virtual EPieceState GetCurrentPieceState() override {return CurrentPieceState;}
	virtual FVector2D GetGridCoordinates() override;

	// Unneeded Interface Implementations
	// Player
	virtual void NotifyEndTurn() override {}
	virtual FVector GetCombatPlayerLocation() override {return FVector::ZeroVector;}
	virtual AActor* GetPlayerCombatant() override {return nullptr;}
	virtual void NotifyMovementRequirementsMet(AActor* BoardPiece) override {}
	virtual void NotifyCombatStatus(int CombatState) override {}
	virtual void SetCombatantCoordinates(FVector2D Coordinates) override {}

	// Gamemode
	virtual void NotifyEndTurnTriggered() override {}
	virtual ETurnOrder GetCurrentTurnOrder() override {return ETurnOrder();}
};

// Created by Snow Paw Games

#pragma once

#include "CombatInterface.h"

#include "CoreMinimal.h"
#include "EnemyTier.h"
#include "Components/ActorComponent.h"
#include "CombatManager.generated.h"

class ABoardPiece;

USTRUCT()
struct FFactionTierContainer
{
	GENERATED_BODY()
	
	UPROPERTY() TArray<AActor*> FactionLeader;
	UPROPERTY() TArray<AActor*>	Lieutenant;
	UPROPERTY() TArray<AActor*> Elite;
	UPROPERTY() TArray<AActor*> Grunt;
};

UENUM(BlueprintType)
enum ETurnOrder
{
	Player UMETA(DisplayName = "Player"),
	Enemy UMETA(DisplayName = "Enemy"),
	Companion UMETA(DisplayName = "Companion"),
	None UMETA(DisplayName = "None")
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TFCOE_API UCombatManager : public UActorComponent, public ICombatInterface
{

private:
	GENERATED_BODY()

public:	
	UCombatManager();
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNewTurnBegin, EFactionID, FactionTurn);
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnNewTurnBegin OnNewTurnBegin;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Settings|Combat State")
	int CurrentCombatState = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Turn Order")
	TMap<int, TEnumAsByte<ETurnOrder>> TurnOrder;

	// The turn priority's for the individual factions on the enemy turn, which ones will go before the others etc... might be subbed for a different system later
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Turn Order")
	TMap<EFactionID, int> FactionTurnPriority;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Settings|Combatants")
	TArray<AActor*> PlayerPartyRoster = {};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Settings|Combatants")
	TArray<AActor*> ActiveCombatantRoster = {};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|UI")
	TSoftClassPtr<UUserWidget> CombatHUD_Class;
	UPROPERTY()
	UUserWidget* CombatHUD;

	// The map that contains the inner scope of the turn order for each faction, based on the actors rank within that faction.
	TMap<EFactionID, FFactionTierContainer> FactionTurnGroups = {};

	// This information is for the turn order as a whole e.g. Player, Companion, Enemy etc...
	ETurnOrder CurrentTurnOrder = None;
	int CurrentTurnIndex = 1;

	// This information is for the enemy turn only. Which factions will execute their turns first. 
	TArray<EFactionID> FactionTurnOrder = {};
	EEnemyTier CurrentFactionRankTurn;
	int CurrentFactionTurnIndex = 0;
	
	// Interface References
	ICombatInterface* CombatInterfacePlayer = nullptr;

public:	
	
	/**
	 * 0 -> Disengaged
	 * 1 -> Engaged
	 * @param CombatState The state to set the combat into. 
	 */
	UFUNCTION(BlueprintCallable, Category="CombatManager")
	void SetCombatState(int CombatState);

	void SetTurnOrder(int NewTurnOrder);
	UFUNCTION(BlueprintCallable, Category="CombatManager")
	void EndCurrentTurn();
	void ExecuteTurnFunctionality(ETurnOrder NewTurn);
	void EndCombat();
	
	void UpdateCombatUI(int CombatState);

	void SetActiveCombatants(const TArray<AActor*>& NewCombatants);
	void AddActiveCombatant(AActor* NewCombatant);
	void ClearActiveCombatants();
	void AddPlayerPartyToActiveCombatants();

	void QueueFactionGroupsForTurn();
	TArray<EFactionID> OrderFactionsForTurn();
	void ExecuteEnemyTurn();
	void ExecuteIndividualEnemyTurn();
	
	void RemoveUnitFromActiveRoster(AActor* UnitToRemove);

	/**
	 * 0 -> Disengaged
	 * 1 -> Engaged
	 */
	UFUNCTION(BlueprintCallable, Category="CombatManager")
	int GetCombatState() const
	{
		return CurrentCombatState;
	}

	UFUNCTION(BlueprintCallable, Category="CombatManager")
	ETurnOrder GetCurrentTurnOrder() const
	{
		return CurrentTurnOrder;
	}

	UFUNCTION(BlueprintCallable, Category="CombatManager")
	int GetCurrentTurnIndex() const
	{
		return CurrentTurnIndex;
	}

	TArray<AActor*> GetActiveCombatants() const
	{
		return ActiveCombatantRoster;
	}
	
	UFUNCTION(blueprintCallable, Category="CombatManager")
	UUserWidget* GetCombatHUD() const
	{
		if (CombatHUD)
		{
			return CombatHUD;
		}
		return nullptr;
	}

	// Interface Implementation
	void NotifyPlayerOfCombatStatus(int CombatState) const;
	virtual ETurnOrder GetCurrentTurnOrder() override {return CurrentTurnOrder;}

	// Unneeded Interface Implementations
	// Player
	virtual void NotifyEndIndividualTurn() override {}
	virtual FVector GetCombatPlayerLocation() override {return FVector::ZeroVector;}
	virtual AActor* GetPlayerCombatant() override {return nullptr;}
	virtual void NotifyMovementRequirementsMet(AActor* BoardPiece) override {}
	virtual void NotifyCombatStatus(int CombatState) override {}
	virtual void SetCombatantCoordinates(FIntPoint Coordinates) override {}
	virtual int GetTimePoints() override {return 0;}
	virtual void SetPlayerHoverMovementModeActive(bool IsActivate) override {}
	virtual void NotifyNewCameraFocus(AActor* Target) override {}
	virtual void NotifyTimePointsExpended(const int32 Amount) override {}

	// Gamemode
	virtual FIntPoint GetGridCoordinates() override {return FIntPoint::ZeroValue;}
	virtual void NotifyEndTurnTriggered() override {}
	virtual void MoveAI_Character(FVector Location) override {}
	virtual AActor* GetGridPieceFromCoordinates(FIntPoint Coordinates) override {return nullptr;}
	virtual void BeginTurnPhase() override {}
	virtual TArray<AActor*> GetActiveCombatantRoster() override {return TArray<AActor*>();}
	virtual EFactionID GetActorFactionID() override {return EFactionID::None;}
	virtual EEnemyTier GetActorFactionRank() override {return EEnemyTier::Grunt;}
	virtual EEntityID GetActorEntityID() override {return EEntityID::Uninitialized_ID;}
	virtual void SetAttackerReference(AActor* AttackerReference) override {}

	// Board Piece
	virtual void NotifyPieceClicked() override {}
	virtual FVector GetBoardPieceLocation() override {return FVector::ZeroVector;}
	virtual EPieceState GetCurrentPieceState() override {return EPieceState();}
	virtual bool DoesGridContainCoordinate(FIntPoint CoordsToCheck) override {return false;}
};

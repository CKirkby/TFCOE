// Created by Snow Paw Games

#pragma once

#include "CoreMinimal.h"
#include "EncounterInterface.h"
#include "GameFramework/Actor.h"
#include "CombatEncounterData.generated.h"

class ABoardPiece;
class UBoxComponent;

UCLASS()
class TFCOE_API ACombatEncounterData : public AActor, public IEncounterInterface
{
	
private:
	GENERATED_BODY()

public:	
	ACombatEncounterData();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UBoxComponent* BoxComponent = nullptr;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	FName EncounterTag = FName("EncounterX");
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Settings")
	TArray<AActor*> CombatBoard = {};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	TArray<AActor*> EnemyCombatants = {};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	ABoardPiece* StartingBoardPiece = nullptr;

public:
	virtual void BeginPlay() override;

	void CollectEncounterPieces();
	void SendEncounterInformation() const;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// Interface Implementation

	// Encounter Interface
	virtual void BeginCombat() override {};
	virtual void InitialiseActiveBoard(TArray<AActor*> ActivePieces) override {};
	virtual void InitialiseActiveCombatants(TArray<AActor*> ActiveCombatants) override{};
	
};

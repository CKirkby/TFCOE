// Created by Snow Paw Games

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "EncounterInterface.generated.h"

UINTERFACE(Blueprintable)
class TFCOE_API UEncounterInterface : public UInterface
{
	GENERATED_BODY()
};

class TFCOE_API IEncounterInterface
{
	GENERATED_BODY()

public:
	
	virtual void BeginCombat() = 0;
	virtual void InitialiseActiveBoard(TArray<AActor*> ActivePieces, AActor* StartingPiece) = 0;
	virtual void InitialiseActiveCombatants(TArray<AActor*> ActiveCombatants) = 0; 
};

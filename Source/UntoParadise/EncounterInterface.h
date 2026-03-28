// Created by Kairo Forge

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "EncounterInterface.generated.h"

class ABoardPiece;

UINTERFACE(Blueprintable)
class UNTOPARADISE_API UEncounterInterface : public UInterface
{
	GENERATED_BODY()
};

class UNTOPARADISE_API IEncounterInterface
{
	GENERATED_BODY()

public:
	
	virtual void BeginCombat() = 0;
	virtual void InitialiseActiveBoard(TArray<AActor*> ActivePieces, AActor* StartingPiece) = 0;
	virtual void InitialiseActiveCombatants(TMap<AActor*, ABoardPiece*> ActiveCombatants) = 0; 
};

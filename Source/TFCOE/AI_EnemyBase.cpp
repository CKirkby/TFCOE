// Created by Snow Paw Games


#include "AI_EnemyBase.h"

#include "AIController.h"
#include "CharacterCombatData.h"

AAI_EnemyBase::AAI_EnemyBase()
{
	CombatData = CreateDefaultSubobject<UCharacterCombatData>(TEXT("Combat Data"));
}

void AAI_EnemyBase::MoveAI_Character(FVector Location)
{
	if (AAIController* AI_Controller = Cast<AAIController>(GetController()))
	{
		// Moves the character. 
		AI_Controller->MoveToLocation(Location, 5.0f, false);
	}
}

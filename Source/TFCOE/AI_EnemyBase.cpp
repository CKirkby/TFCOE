// Created by Snow Paw Games


#include "AI_EnemyBase.h"

#include "CharacterCombatData.h"

AAI_EnemyBase::AAI_EnemyBase()
{
	CombatData = CreateDefaultSubobject<UCharacterCombatData>(TEXT("Combat Data"));
}

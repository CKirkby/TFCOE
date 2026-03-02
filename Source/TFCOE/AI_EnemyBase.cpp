// Created by Snow Paw Games


#include "AI_EnemyBase.h"

#include "AIController.h"
#include "CharacterCombatData.h"
#include "HealthComponent.h"
#include "Components/WidgetComponent.h"

AAI_EnemyBase::AAI_EnemyBase()
{
	CombatData = CreateDefaultSubobject<UCharacterCombatData>(TEXT("Combat Data"));
	HealthComp = CreateDefaultSubobject<UHealthComponent>(TEXT("Health Component"));
	
	
	// Reminder to set the material to translucent or the animation doesn't work
	TargetIndicatorWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Target Indicator Widget"));
	TargetIndicatorWidget->SetupAttachment(GetRootComponent());
	TargetIndicatorWidget->SetWidgetSpace(EWidgetSpace::World);
	TargetIndicatorWidget->SetHiddenInGame(true);
}

void AAI_EnemyBase::BeginPlay()
{
	Super::BeginPlay();

	InitialiseHealth();
}

// Interface call to move the current character
void AAI_EnemyBase::MoveAI_Character(FVector Location)
{
	if (AAIController* AI_Controller = Cast<AAIController>(GetController()))
	{
		// Moves the character. 
		AI_Controller->MoveToLocation(Location, 5.0f, false);
	}
}

// Interface call to execute the current turn, will be called when its their turn. 
void AAI_EnemyBase::BeginTurnPhase()
{
	CombatData->ExecuteCurrentTurn();
}

// Interface call to get the faction ID from the configuration in the combat data. 
EFactionID AAI_EnemyBase::GetActorFactionID()
{
	if (CombatData)
	{
		return CombatData->GetFactionID();
	}

	return EFactionID::None;
}

EEnemyTier AAI_EnemyBase::GetActorFactionRank()
{
	if (CombatData)
	{
		return CombatData->GetFactionRank();
	}

	return EEnemyTier::Grunt;
}

void AAI_EnemyBase::SetAttackerReference(AActor* AttackerReference)
{
	if (AttackerReference && CombatData)
	{
		CombatData->SetPreviousAttacker(AttackerReference);
	}
}

void AAI_EnemyBase::InitialiseHealth() const
{
	// Gets the set health from the enemy configuration and inputs it into the health comp.
	HealthComp->InitialiseHealth(CombatData->GetHealthConfig());
}

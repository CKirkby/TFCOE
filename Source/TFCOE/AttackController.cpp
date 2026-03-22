// Created by Snow Paw Games


#include "AttackController.h"
#include "AttackConfiguration.h"
#include "CombatInterface.h"
#include "HealthInterface.h"
#include "Kismet/KismetMathLibrary.h"

UAttackController::UAttackController()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UAttackController::BeginPlay()
{
	Super::BeginPlay();
	
}

void UAttackController::PerformBasicAttack(AActor* Target, const FAttackConfiguration* ChosenAttack) const
{
	if (!Target || !PlayerCombatantObjRef) return;
	
	// Sets the Targets rotation to face the attacker
	const FRotator Target_TargetRotation = UKismetMathLibrary::FindLookAtRotation(Target->GetActorLocation(), PlayerCombatantObjRef->GetActorLocation());
	Target->SetActorRotation(Target_TargetRotation);
		
	//Uses attack hit chance to see if it hits or not, Converts percent to decimal (80 - 0.8)
	if (UKismetMathLibrary::RandomBoolWithWeight(ChosenAttack->BaseHitChance / 100))
	{
		// Functionality to remove health. 
		IHealthInterface* HealthInterface = Cast<IHealthInterface>(Target);
		if (!HealthInterface) return;
		HealthInterface->TakeDamage(ChosenAttack->AttackDamage);
			
		//TODO - Make this function into a faction ID Switch to set attacker, I dont like how linear this is
		// Sets the attacker reference of the target to this attacker
		SetAttackerReference(PlayerCombatantObjRef);
	}
}

void UAttackController::SetAttackerReference(AActor* Target) const
{
	AActor* Owner = GetOwner();
	if (Target && Owner)
	{
		ICombatInterface* CombatInterface = Cast<ICombatInterface>(Target);
		if (!CombatInterface) return;
		
		CombatInterface->SetAttackerReference(Owner);
	}
}



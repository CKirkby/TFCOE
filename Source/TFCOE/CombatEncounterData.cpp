// Created by Snow Paw Games


#include "CombatEncounterData.h"
#include "GameFramework/GameModeBase.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ACombatEncounterData::ACombatEncounterData()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>("Root");

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Collider"));
	BoxComponent->SetupAttachment(RootComponent);
}

void ACombatEncounterData::BeginPlay()
{
	Super::BeginPlay();

	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &ACombatEncounterData::OnOverlapBegin);

	// Gets all the board pieces for this encounter and stores them.
	CollectEncounterPieces();
}

void ACombatEncounterData::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                          UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor->ActorHasTag("Player"))
	{
		// Starts the combat and sends the board and combatants.
		if (!CombatTriggered)
		{
			SendEncounterInformation();
			SetActorEnableCollision(false);
			CombatTriggered = true;
		}
	}
}

void ACombatEncounterData::CollectEncounterPieces()
{
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), EncounterTag, CombatBoard);
}

void ACombatEncounterData::SendEncounterInformation() const
{
	// Accesses the gamemode's interface
	if (IEncounterInterface* EncounterInterface = Cast<IEncounterInterface>(UGameplayStatics::GetGameMode(GetWorld())))
	{
		// Tells the gamemode to start combat 
		EncounterInterface->BeginCombat();

		// Sends the combat pieces to the board manager in the gamemode
		if (!CombatBoard.IsEmpty())
		{
			EncounterInterface->InitialiseActiveBoard(CombatBoard);
		}

		// Sends the active combatants for this combat instance to the combat manager
		if (!EnemyCombatants.IsEmpty())
		{
			EncounterInterface->InitialiseActiveCombatants(EnemyCombatants);
		}
	}
}

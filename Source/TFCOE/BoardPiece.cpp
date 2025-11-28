// Created by Snow Paw Games

#include "BoardPiece.h"

#include "Components/BoxComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Engine/World.h"

ABoardPiece::ABoardPiece()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));
	StaticMesh->SetupAttachment(RootComponent);
	
	CharacterPosition = CreateDefaultSubobject<USceneComponent>(TEXT("Character Position"));
	CharacterPosition->SetupAttachment(StaticMesh);		
}

void ABoardPiece::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	// Binds the on overlap function
	if (BoxCollision)
	{
		BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &ABoardPiece::OnOverlapBegin);
        BoxCollision->OnComponentEndOverlap.AddDynamic(this, &ABoardPiece::OnOverlapEnd);
	}
}

void ABoardPiece::BeginPlay()
{
	Super::BeginPlay();
}

void ABoardPiece::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Error, TEXT("Something overlapped"))
	
	if (!OtherActor) return;

	if (OtherActor->ActorHasTag("Combatant"))
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Actor Overlapped: " + OtherActor->GetName()));
		
		// Registers a combatant has entered the zone
		SetPieceState(Occupied);

		if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(OtherActor))
		{
			// Sets the combatants current coordinates and stores which actor is stood upon this piece.
			CombatInterface->SetCombatantCoordinates(GetGridCoordinates());
			SetCurrentOccupier(OtherActor);
		}
	}
}

void ABoardPiece::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	UE_LOG(LogTemp, Error, TEXT("Something ended overlap"))

	if (!OtherActor) return;

	if (OtherActor->ActorHasTag("Combatant"))
	{
		GEngine->AddOnScreenDebugMessage(-1, 0.05f, FColor::Green, TEXT("Actor Left overlap: " + OtherActor->GetName()));
		
		// Returns the piece state to 
		SetPieceState(Enabled);
		ClearCurrentOccupier();
	}
}

FVector ABoardPiece::GetBoardPieceLocation()
{
	if (CharacterPosition)
	{
		return CharacterPosition->GetComponentLocation();
	}

	return FVector::ZeroVector;
}

// An interface function to return the coordinates that were set
FVector2D ABoardPiece::GetGridCoordinates()
{
	return GridPosition;
}

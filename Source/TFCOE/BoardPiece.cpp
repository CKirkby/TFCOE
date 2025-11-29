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

void ABoardPiece::BeginPlay()
{
	Super::BeginPlay();

	// Binds the on overlap function
	if (BoxCollision)
	{
		BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &ABoardPiece::OnOverlapBegin);
		BoxCollision->OnComponentEndOverlap.AddDynamic(this, &ABoardPiece::OnOverlapEnd);
	}
}

void ABoardPiece::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor) return;

	if (OtherActor->ActorHasTag("Combatant"))
	{
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
	if (!OtherActor) return;

	if (OtherActor->ActorHasTag("Combatant"))
	{		
		// Returns the piece state to 
		SetPieceState(Enabled);
		ClearCurrentOccupier();
	}
}

void ABoardPiece::NotifyPieceClicked()
{
	if (!OriginalMaterial || !MovementClickMaterial)
	{
		UE_LOG(LogTemp, Error, TEXT("Board Piece: Piece clicked - One or more materials not set"))
		return;
	}

	UMaterialInstanceDynamic* NewMaterial = UMaterialInstanceDynamic::Create(MovementClickMaterial, this);
	StaticMesh->SetMaterial(0, NewMaterial);

	// Sets a timer to reset the material
	FTimerHandle DelayBeforeResetMaterialHandle;
	GetWorld()->GetTimerManager().SetTimer(DelayBeforeResetMaterialHandle,this, &ABoardPiece::ResetMaterial, 1.0f, false);
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

void ABoardPiece::ResetMaterial()
{
	GEngine->AddOnScreenDebugMessage(-1, 0.05f, FColor::Green, TEXT("This fired"));
	UMaterialInstanceDynamic* OrginalMat = UMaterialInstanceDynamic::Create(OriginalMaterial, this);
	StaticMesh->SetMaterial(0, OrginalMat);
}

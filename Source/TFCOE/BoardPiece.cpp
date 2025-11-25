// Created by Snow Paw Games

#include "BoardPiece.h"

#include "Components/BoxComponent.h"
#include "Engine/World.h"
#include "Kismet/KismetSystemLibrary.h"

ABoardPiece::ABoardPiece()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));
	StaticMesh->SetupAttachment(RootComponent);

	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision Box"));
	BoxCollision->SetupAttachment(RootComponent);
	
	CharacterPosition = CreateDefaultSubobject<USceneComponent>(TEXT("Character Position"));
	CharacterPosition->SetupAttachment(RootComponent);
}

void ABoardPiece::BeginPlay()
{
	Super::BeginPlay();
}

// An interface function to return the coordinates that were set
FVector2D ABoardPiece::GetGridCoordinates()
{
	return GridPosition;
}

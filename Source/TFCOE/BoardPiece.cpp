// Created by Snow Paw Games

#include "BoardPiece.h"
#include "Engine/World.h"
#include "Kismet/KismetSystemLibrary.h"

ABoardPiece::ABoardPiece()
{

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

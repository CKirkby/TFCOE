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

TArray<FHitResult> ABoardPiece::ActivateCombatTrace(int Directions)
{
	TArray<FVector> DirectionsToTrace;

	// Cardinal Directions
	const FVector UpVector = FVector(0, 1, 0);
	const FVector DownVector = FVector(0, -1, 0);
	const FVector RightVector = FVector(1, 0, 0);
	const FVector LeftVector = FVector(-1, 0, 0);

	// Ordinal Directions
	const FVector UpperLeftVector = FVector(-1, 1, 0).GetSafeNormal();
	const FVector UpperRightVector = FVector(1, 1, 0).GetSafeNormal();
	const FVector LowerLeftVector = FVector(-1, -1, 0).GetSafeNormal();
	const FVector LowerRightVector = FVector(1, -1, 0).GetSafeNormal();

	// Creates a mini binding function that if called, adds the direction linked to the directions to be checked. 
	const ETraceDirections TraceFlags = static_cast<ETraceDirections>(Directions);
	const TArray<FDirectionBinding> DirectionBindings =
		{
		{ETraceDirections::Up, [&]() {DirectionsToTrace.Add(UpVector);}}, //Adds the up direction
		{ETraceDirections::Down, [&]() {DirectionsToTrace.Add(DownVector);}}, // Adds the down direction
		{ETraceDirections::Right, [&]() {DirectionsToTrace.Add(RightVector);}}, // Adds the right vector
		{ETraceDirections::Left, [&]() {DirectionsToTrace.Add(LeftVector);}}, // Adds the left vector
		{ETraceDirections::UpperLeft, [&]() {DirectionsToTrace.Add(UpperLeftVector);}}, // Adds the upper left vector
		{ETraceDirections::UpperRight, [&]() {DirectionsToTrace.Add(UpperRightVector);}}, // Adds the upper right vector
		{ETraceDirections::LowerLeft, [&]() {DirectionsToTrace.Add(LowerLeftVector);}}, // Adds the lower left vector
		{ETraceDirections::LowerRight, [&]() {DirectionsToTrace.Add(LowerRightVector);}} // Adds the lower right vector
	};

	// Checks which flag directions were set and triggers the action to bind it to the array of directions to check.
	for (const FDirectionBinding& Binding : DirectionBindings)
	{
		if (EnumHasAllFlags(TraceFlags, Binding.Direction))
		{
			Binding.BindAction();
		}
	}

	TArray<FHitResult> HitResults;

	// Does a trace for each direction in the array to be checked
	for (FVector Direction : DirectionsToTrace)
	{
		FVector TraceStart = GetActorLocation();
		FVector TraceEnd = TraceStart + Direction * 350.0f;

		TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
		ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));
		
		UKismetSystemLibrary::LineTraceMultiForObjects(GetWorld(), TraceStart, TraceEnd, ObjectTypes,
			false, TArray<AActor*>(), EDrawDebugTrace::ForDuration, HitResults, true);
	}
	
	return HitResults;
}

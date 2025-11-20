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

TArray<FHitResult> ABoardPiece::ActivateCombatTrace(int Directions, const float CardinalTraceSize, const float OrdinalTraceSize)
{
	// Per cube for cardinal direction is 150 trace size.
	// Per cube for ordinal direction is 
	
	TArray<FVector> CardinalDirectionsToTrace;
	TArray<FVector> OrdinalDirectionsToTrace;

	// Cardinal Directions
	const FVector UpVector = FVector(0, -1, 0);
	const FVector DownVector = FVector(0, 1, 0);
	const FVector RightVector = FVector(1, 0, 0);
	const FVector LeftVector = FVector(-1, 0, 0);

	// Ordinal Directions
	const FVector UpperLeftVector = FVector(-1, -1, 0).GetSafeNormal();
	const FVector UpperRightVector = FVector(1, -1, 0).GetSafeNormal();
	const FVector LowerLeftVector = FVector(-1, 1, 0).GetSafeNormal();
	const FVector LowerRightVector = FVector(1, 1, 0).GetSafeNormal();

	// Creates a mini binding function that if called, adds the direction linked to the directions to be checked. 
	const ETraceDirections TraceFlags = static_cast<ETraceDirections>(Directions);
	const TArray<FDirectionBinding> DirectionBindings =
		{
		{ETraceDirections::Up, [&]() {CardinalDirectionsToTrace.Add(UpVector);}}, //Adds the up direction
		{ETraceDirections::Down, [&]() {CardinalDirectionsToTrace.Add(DownVector);}}, // Adds the down direction
		{ETraceDirections::Right, [&]() {CardinalDirectionsToTrace.Add(RightVector);}}, // Adds the right vector
		{ETraceDirections::Left, [&]() {CardinalDirectionsToTrace.Add(LeftVector);}}, // Adds the left vector
		{ETraceDirections::UpperLeft, [&]() {OrdinalDirectionsToTrace.Add(UpperLeftVector);}}, // Adds the upper left vector
		{ETraceDirections::UpperRight, [&]() {OrdinalDirectionsToTrace.Add(UpperRightVector);}}, // Adds the upper right vector
		{ETraceDirections::LowerLeft, [&]() {OrdinalDirectionsToTrace.Add(LowerLeftVector);}}, // Adds the lower left vector
		{ETraceDirections::LowerRight, [&]() {OrdinalDirectionsToTrace.Add(LowerRightVector);}} // Adds the lower right vector
	};

	// Checks which flag directions were set and triggers the action to bind it to the array of directions to check.
	for (const FDirectionBinding& Binding : DirectionBindings)
	{
		if (EnumHasAllFlags(TraceFlags, Binding.Direction))
		{
			Binding.BindAction();
		}
	}

	const TArray<FHitResult> CardinalHitResults = PerformDirectionalTrace(CardinalDirectionsToTrace, CardinalTraceSize);
	const TArray<FHitResult> OrdinalHitResults = PerformDirectionalTrace(OrdinalDirectionsToTrace, OrdinalTraceSize);

	TArray<FHitResult> FinalHitResults;
	FinalHitResults.Append(CardinalHitResults);
	FinalHitResults.Append(OrdinalHitResults);
	
	return FinalHitResults;
}

TArray<FHitResult> ABoardPiece::PerformDirectionalTrace(TArray<FVector> DirectionArray, const float TraceSize)
{
	if (!DirectionArray.IsEmpty())
	{
		TArray<FHitResult> HitResults;
		
		for (FVector Direction : DirectionArray)
		{
			FVector TraceStart = GetActorLocation();
			FVector TraceEnd = TraceStart + Direction * TraceSize;

			TArray<AActor*> ActorsToIgnore;
			ActorsToIgnore.Add(this);

			TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
			ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));
		
			UKismetSystemLibrary::LineTraceMultiForObjects(GetWorld(), TraceStart, TraceEnd, ObjectTypes,
			   false, ActorsToIgnore, EDrawDebugTrace::Persistent, HitResults, true);
		}

		return HitResults;
	}

	return TArray<FHitResult>();
}

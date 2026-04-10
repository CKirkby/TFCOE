// Created by Kairo Forge

#include "BoardPiece.h"

#include "NiagaraComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Components/WidgetComponent.h"
#include "Engine/World.h"

ABoardPiece::ABoardPiece()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));
	StaticMesh->SetupAttachment(RootComponent);
	
	CharacterPosition = CreateDefaultSubobject<USceneComponent>(TEXT("Character Position"));
	CharacterPosition->SetupAttachment(StaticMesh);
	
	// Reminder to set the material to translucent or the animation doesn't work
	TargetIndicatorWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Target Indicator Widget"));
	TargetIndicatorWidget->SetupAttachment(GetRootComponent());
	TargetIndicatorWidget->SetWidgetSpace(EWidgetSpace::World);
	TargetIndicatorWidget->SetHiddenInGame(true);
	
	DamageIndicator = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Damage Indicator"));
	DamageIndicator->SetupAttachment(RootComponent);
}

void ABoardPiece::BeginPlay()
{
	Super::BeginPlay();
}

void ABoardPiece::TriggerOverlapFunctionality(AActor* OtherActor)
{
	if (!OtherActor) return;
	if (CurrentPieceState == EPieceState::Disabled) return;

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

void ABoardPiece::TriggerOverlapEndFunctionality(AActor* OtherActor)
{
	if (!OtherActor) return;
	if (CurrentPieceState == EPieceState::Disabled) return;

	if (OtherActor->ActorHasTag("Combatant"))
	{		
		// Returns the piece state to normal 
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

void ABoardPiece::NotifyBoardPieceHighlight(EHighlightType Type)
{
	switch (Type)
	{
	case EHighlightType::None:
		{
			//Resets the material to its default state
			ResetMaterial();
			break;
		}
		
	case EHighlightType::Movement:
		{
			// Creates the material instance for the reachable movement position material. 
			UMaterialInstanceDynamic* MovementHighlightMat = UMaterialInstanceDynamic::Create(ReachableMovementMaterial, this);
			StaticMesh->SetMaterial(0, MovementHighlightMat);
			break;
		}
		
	case EHighlightType::Attack:
		{
			UMaterialInstanceDynamic* AttackHighlightMat = UMaterialInstanceDynamic::Create(AttackHighlightMaterial, this);
			StaticMesh->SetMaterial(0, AttackHighlightMat);
			break;
		}
	}
}

void ABoardPiece::NotifyBoardPieceOnHover()
{
	if (TargetIndicatorWidget)
	{
		TargetIndicatorWidget->SetHiddenInGame(false);
	}
}

void ABoardPiece::NotifyBoardPieceOnHoverEnd()
{
	if (TargetIndicatorWidget)
	{
		TargetIndicatorWidget->SetHiddenInGame(true);
	}
}

void ABoardPiece::ResetMaterial()
{
	UMaterialInstanceDynamic* OriginalMat = UMaterialInstanceDynamic::Create(OriginalMaterial, this);
	StaticMesh->SetMaterial(0, OriginalMat);
}

void ABoardPiece::SetGridCoordinates(const FIntPoint NewCoordinates)
{
	GridPosition = NewCoordinates;
}

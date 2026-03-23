// Created by Snow Paw Games


#include "PlayerCharacter.h"

#include "AttackController.h"
#include "BoardControllerInterface.h"
#include "BoardPiece.h"
#include "Character_Inventory.h"
#include "CombatCameraOperator.h"
#include "CombatInterface.h"
#include "CombatManager.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/GameModeBase.h"
#include "Engine/AssetManager.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"


APlayerCharacter::APlayerCharacter()
{
	CharacterInventory = CreateDefaultSubobject<UCharacter_Inventory>(TEXT("Character Inventory"));
	CombatData = CreateDefaultSubobject<UCharacterCombatData>(TEXT("Combat Data"));
	AttackController = CreateDefaultSubobject<UAttackController>(TEXT("Attack Controller"));
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Gets the movement component and stores it for future use.
	InitialiseMovementComponent();

	PlayerController = Cast<APlayerController>(GetController());
}

// Initialises the player input system
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (const APlayerController* InitPlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(InitPlayerController->GetLocalPlayer()))
		{
			if (MappingContext)
			{
				Subsystem->AddMappingContext(MappingContext, 0);
			}
		}
	}

	// Initialises the action bindings 
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Insert Action bindings here

		// Binds the movement action to the input variables
		if (MoveAction)
		{
			EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerCharacter::MoveTrigger);
			EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Canceled, this, &APlayerCharacter::ResetMovement);
			EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &APlayerCharacter::ResetMovement);
		}

		// Binds the interact action to the triggered function
		if (InteractAction)
		{
			EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &APlayerCharacter::InteractTrigger);
		}

		// Binds the sprint action to the trigger for starting, and the completed to the end function
		if (SprintAction)
		{
			EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &APlayerCharacter::SprintTrigger);
			EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &APlayerCharacter::SprintEnd);
			EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Canceled, this, &APlayerCharacter::SprintEnd);
		}

		if (CombatClickAction)
		{
			EnhancedInputComponent->BindAction(CombatClickAction, ETriggerEvent::Started, this, &APlayerCharacter::CombatClickTrigger);
		}

		if (EndTurnAction)
		{
			EnhancedInputComponent->BindAction(EndTurnAction, ETriggerEvent::Triggered, this, &APlayerCharacter::EndTurnTrigger);
		}
		
	} 
}

void APlayerCharacter::InitialiseMovementComponent()
{
	// Gets and caches the movement component for use 
	if (UCharacterMovementComponent* CharacterMovementComp = GetCharacterMovement())
	{
		MovementComponent = CharacterMovementComp;
	}
}

void APlayerCharacter::UpdatePlayerCombatState(const bool CombatEnabled)
{
	if (PlayerController)
	{
		if (CombatEnabled)
		{
			EnterCombatMode();
		}
		else
		{
			ExitCombatMode();
		}
	}
}

void APlayerCharacter::MoveTrigger(const FInputActionValue& Value)
{
	const FVector2D MovementVector = Value.Get<FVector2D>();

	if (CameraMovementEnabled)
	{
		if (CameraOperator)
		{
			CameraOperator->AddMovementInput(FVector(1, 0, 0), MovementVector.X);
			CameraOperator->AddMovementInput(FVector(0, 1, 0), MovementVector.Y);
		}
	}
	
	if (MovementEnabled)
	{
		// Movement for X vector
		AddMovementInput(FVector(1, 0, 0), MovementVector.X);

		// Movement for Y vector
		AddMovementInput(FVector(0, 1, 0), MovementVector.Y);
	}
}

void APlayerCharacter::ResetMovement()
{
	MovementComponent->StopMovementImmediately();
}

void APlayerCharacter::SprintTrigger()
{
	if (MovementComponent)
	{
		// Sets the movement speed to be the sprint speed on sprint start
		MovementComponent->MaxWalkSpeed = SprintSpeed;
	}
}

void APlayerCharacter::SprintEnd()
{
	if (MovementComponent)
	{
		// Sets the movement speed to be the walk speed on the sprint end
		MovementComponent->MaxWalkSpeed = WalkSpeed;
	}
}

void APlayerCharacter::InteractTrigger()
{
	UE_LOG(LogTemp, Warning, TEXT("The player has interacted with something"))
	CharacterInventory->AddToGeneralItems("Scrap", FMath::RandRange(1, 4));

	const int* Amount = CharacterInventory->GetGeneralItemList().Find("Scrap");
	UE_LOG(LogTemp, Error, TEXT("You now have %i Scrap metal"), *Amount)
}

void APlayerCharacter::CombatClickTrigger()
{
	if (PlayerController && CombatModeActivated)
	{
		switch (CurrentPlayerTurnState)
		{
		case EPlayerTurnState::Neutral:
			break;
			
		case EPlayerTurnState::MovementMode:
			
			if (CurrentGridPieceHovered)
			{
				// Broadcasts back to blueprints which was hit, where then I can do checks etc...
				OnBoardPieceClicked(CurrentGridPieceHovered);	
			}
			
			break;
			
		case EPlayerTurnState::CombatMode:
			if (CurrentTargetHovered && IsTargetWithinRange(CurrentTargetHovered))
			{
				// Broadcast to Attack Enemy
				OnTargetCombatantClicked(CurrentTargetHovered);
			}
			
			break;
		}
	}
}

void APlayerCharacter::OnBoardPieceClicked(AActor* BoardPiece)
{
	if (!BoardPiece) return;

	// Checking if the player can initiate movement
	if (CombatModeActivated && CurrentPlayerTurnState == EPlayerTurnState::MovementMode && CheckIsPlayersTurn() && CheckGridSlotAvailable(BoardPiece))
	{
		if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetPlayerCombatant_Vague()))
		{
			// If it can, tells the AI player so that it can initiate movement.
			CombatInterface->NotifyMovementRequirementsMet(BoardPiece);
			
			// Notifies delegates of successful movement.
			OnSuccessfulMovementTriggered();
		}
		
		// Resets any and all highlighted movement pieces on successful click.
		if (IBoardControllerInterface* BC_Interface = Cast<IBoardControllerInterface>(UGameplayStatics::GetGameMode(GetWorld())))
		{
			// Turns off all the systems relating to movement whilst the player is moving, turn back on, on complete. 
			ExitHoverMode();
			NotifyGridOnHoverEnd();
			BC_Interface->ResetHighlightedPieces();
		}
	}
}

void APlayerCharacter::OnTargetCombatantClicked(const AActor* Target)
{
	if (!Target || !CurrentTargetHovered || !PlayerCombatant) return;
	
	// Checking if the player can initiate movement
	if (CombatModeActivated && CurrentPlayerTurnState == EPlayerTurnState::CombatMode && CheckIsPlayersTurn() && Target)
	{
		if (CurrentTargetHovered == Target)
		{
			const FAttackConfiguration* AttackConfiguration = GetAttackConfig("Player_BasicMelee");
			
			AttackController->PerformBasicAttack(CurrentTargetHovered, AttackConfiguration);
			
			// Notify the blueprint of an attack performed
			PlayerCombatant->CommenceBasicAttack(AttackConfiguration, true);
			
			// Resets
			ExitHoverMode();
		}
	}
}

bool APlayerCharacter::CheckGridPieceActive(AActor* TargetPiece)
{
	// Performs a check to make sure it is targetable
	if (!TargetPiece) return false;
	
	if (ICombatInterface* CombatInterfaceGrid = Cast<ICombatInterface>(TargetPiece))
	{
		const EPieceState State = CombatInterfaceGrid->GetCurrentPieceState();
	
		// Checks the piece isn't occupied or non-walkable.
		return State != EPieceState::Occupied && State != EPieceState::Disabled;
	}
	
	return false;
}

FAttackConfiguration* APlayerCharacter::GetAttackConfig(const FName AttackID) const
{
	if (!PlayerCombatant || AttackID.IsNone()) return nullptr;
	
	// Gets the attacks that the player has stored before use so we can find the one we want. 
	TArray<FAttackConfiguration> CachedAttackConfigs = PlayerCombatant->GetUnitConfiguration()->AttackConfigurations;
	
	// Check to make sure the array is not empty before use
	if (CachedAttackConfigs.IsEmpty()) return nullptr;

	for (auto& CachedAttackConfig : CachedAttackConfigs)
	{
		if (AttackID == CachedAttackConfig.AttackID)
		{
			return &CachedAttackConfig;
		}
	}
	
	return nullptr;
}

void APlayerCharacter::EnterHoverMode()
{
	HoverModeActive = true;
	
	IBoardControllerInterface* BC_Interface = Cast<IBoardControllerInterface>(UGameplayStatics::GetGameMode(GetWorld()));
	if (!BC_Interface) return;
	
	FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	
	switch (CurrentPlayerTurnState)
	{
	case EPlayerTurnState::Neutral:
		// Return on neutral, do not want any functionality.
		return;
		
	case EPlayerTurnState::MovementMode:
		// Calculate Movement squares for visuals.
		BC_Interface->SetReachableMovementPositionsVisible(true);
		
		// TODO - Highlight functions not working
		
		// Checks that there isn't a timer active
		if (TimerManager.TimerExists(HoverModeHandle)) TimerManager.ClearTimer(HoverModeHandle);
		
		// Sets a timer to check for grid pieces
		GetWorld()->GetTimerManager().SetTimer(HoverModeHandle, this, &APlayerCharacter::CheckHover_Movement, 0.03f, true);
		break;
		
	case EPlayerTurnState::CombatMode:
		// Calculate visible combat squares
		// Code here
		
		// Checks that there isn't a timer active
		if (TimerManager.TimerExists(HoverModeHandle)) TimerManager.ClearTimer(HoverModeHandle);
		
		GetWorld()->GetTimerManager().SetTimer(HoverModeHandle, this, &APlayerCharacter::CheckHover_Enemy, 0.03f, true);
		break;
	}
}

void APlayerCharacter::CheckHover_Movement()
{
	// Checks what the mouse is clicking on, the aim is to detect board pieces only
	FHitResult HitResult;
	PlayerController->GetHitResultUnderCursorByChannel(static_cast<ETraceTypeQuery>(ECC_GameTraceChannel1), false, HitResult);

	if (HitResult.bBlockingHit)
	{
		AActor* HitTarget = HitResult.GetActor();
		if (!HitTarget) return;
		
		if (HitTarget->ActorHasTag("Grid"))
		{
			// Does a check to make sure the target piece is actually walkable, dont want to target inactive grid pieces.
			if (!CheckGridPieceActive(HitTarget))
			{
				if (CurrentGridPieceHovered)
				{
					NotifyGridOnHoverEnd();
					CurrentGridPieceHovered = nullptr;
				}
				
				return;
			}
			
			IBoardControllerInterface* BC_InterfaceTarget = Cast<IBoardControllerInterface>(HitTarget);
			if (!BC_InterfaceTarget) return;
			
			if (!CurrentGridPieceHovered || HitTarget != CurrentGridPieceHovered)
			{
				NotifyGridOnHoverEnd();
				
				// Set the current hovered piece to the current hovered
				CurrentGridPieceHovered = HitTarget;
				
				// Notify the piece of hovering
				BC_InterfaceTarget->NotifyBoardPieceOnHover();
			}
		}
		else
		{
			NotifyGridOnHoverEnd();
		}
	}
	else
	{
		// If the mouse loses blocking, remove the movement indicator
		NotifyGridOnHoverEnd();
	}
}

void APlayerCharacter::CheckHover_Enemy()
{
	// Checks what the mouse is hovering over, the aim is to detect enemies.
	FHitResult HitResult;
	PlayerController->GetHitResultUnderCursorByChannel(
		UEngineTypes::ConvertToTraceType(ECC_Pawn),false,HitResult);
	
	if (HitResult.bBlockingHit)
	{
		AActor* HitActor = HitResult.GetActor();
		if (!HitActor) return;
		
		if (HitActor->ActorHasTag("Enemy"))
		{
			IBoardControllerInterface* BC_InterfaceTarget = Cast<IBoardControllerInterface>(HitActor);
			ICombatInterface* CombatInterfaceTarget = Cast<ICombatInterface>(HitActor);	
			if (!BC_InterfaceTarget || !CombatInterfaceTarget) return;

			if (!CurrentTargetHovered || HitActor != CurrentTargetHovered)
			{
				
				// If for some reason the player combatant doesn't exist, return
				if (!PlayerCombatant) return;
				
				// Performs a check to make sure that the target is within attack range
				if (IsTargetWithinRange(HitActor))
				{
					// Set the current hovered piece to the current hovered
					CurrentTargetHovered = HitActor;
					
					// Notify the piece of hovering
					BC_InterfaceTarget->NotifyTargetOnHover();
				}
			}
		}
		else
		{
			NotifyTargetOnHoverEnd();
			CurrentTargetHovered = nullptr;
		}
	}
	else
	{
		NotifyTargetOnHoverEnd();
		CurrentTargetHovered = nullptr;
	}
}

void APlayerCharacter::ExitHoverMode()
{
	HoverModeActive = false;
	
	// Calls the functionality to turn off any and all hovers if need be. 
	IBoardControllerInterface* BC_Interface = Cast<IBoardControllerInterface>(UGameplayStatics::GetGameMode(GetWorld()));
	if (!BC_Interface) return;
	BC_Interface->SetReachableMovementPositionsVisible(false);
	
	GetWorld()->GetTimerManager().ClearTimer(HoverModeHandle);
	
	// Turns off combat modes on end turn. 
	LastTurnStateUsed = CurrentPlayerTurnState;
	CurrentPlayerTurnState = EPlayerTurnState::Neutral;
	
	// Clears any cached references to last hovered actors 
	ClearCachedHover();
}

void APlayerCharacter::ClearCachedHover()
{
	if (CurrentGridPieceHovered)
	{
		NotifyGridOnHoverEnd();
		
		CurrentGridPieceHovered = nullptr;
	}
	
	if (CurrentTargetHovered)
	{
		if (IBoardControllerInterface* BC_Interface = Cast<IBoardControllerInterface>(CurrentTargetHovered))
		{
			BC_Interface->NotifyTargetOnHoverEnd();
		}
		
		CurrentTargetHovered = nullptr;
	}
}

void APlayerCharacter::NotifyGridOnHoverEnd() const
{
	if (CurrentGridPieceHovered)
	{
		IBoardControllerInterface* BC_InterfaceCurrent = Cast<IBoardControllerInterface>(CurrentGridPieceHovered);
		if (!BC_InterfaceCurrent) return;
					
		// If there was already a grid piece hovered then that means a new hover has been done, notify no longer hovering
		BC_InterfaceCurrent->NotifyBoardPieceOnHoverEnd();
	}
}

void APlayerCharacter::NotifyTargetOnHoverEnd() const
{
	if (CurrentTargetHovered)
	{
		IBoardControllerInterface* BC_InterfaceCurrent = Cast<IBoardControllerInterface>(CurrentTargetHovered);
		if (!BC_InterfaceCurrent) return;
					
		// If there was already a grid piece hovered then that means a new hover has been done, notify no longer hovering
		BC_InterfaceCurrent->NotifyTargetOnHoverEnd();
	}
}

void APlayerCharacter::EndTurnTrigger()
{
	if (CombatModeActivated)
	{
		if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(UGameplayStatics::GetGameMode(GetWorld())))
		{
			CombatInterface->NotifyEndTurnTriggered();
		}
		
		// Turns off combat modes on end turn. 
		CurrentPlayerTurnState = EPlayerTurnState::Neutral;
	}
}

int32 APlayerCharacter::GetGridDistanceAllDir(const FIntPoint& PointA, const FIntPoint& PointB)
{
	// Uses the Chebyshev method to include Diagonals into the movement consideration
	return FMath::Max(FMath::Abs(PointA.X - PointB.X), FMath::Abs(PointA.Y - PointB.Y));
}

bool APlayerCharacter::IsTargetWithinRange(AActor* Target)
{
	if (!Target || !PlayerCombatant) return false;
	
	// Gets the interfaces for the target
	ICombatInterface* TargetInterface = Cast<ICombatInterface>(Target);
	if (!TargetInterface) return false;
	
	// Checks to see if the target is 1 block away, this means they are in melee range.
	if (GetGridDistanceAllDir(PlayerCombatant->GetGridCoordinates(), TargetInterface->GetGridCoordinates()) == 1)
	{
		return true;
	}
	
	// If all else fails
	return false;
}

void APlayerCharacter::EnterCombatMode()
{
	// The combat mode is enabled, the player will no longer be visible and the dummy player will activate
	CombatModeActivated = true;
	SetLocomotion(false, true);
	PlayerController->bShowMouseCursor = true;

	// Loads the AI Dummy.
	AsyncLoadDummy();

	// Spawns the camera actor and sets the camera to this. 
	SpawnAndSetCameraOperator();

	// Sets the main player to be hidden in the game so that it cannot interfere
	SetMainActorHidden(true);
}

void APlayerCharacter::ExitCombatMode()
{
	// Combat is over or not activated, the player will return to visibility and control restored. 
	CombatModeActivated = false;
	SetLocomotion(true, false);
	PlayerController->bShowMouseCursor = false;

	// Sets the invisible player to the dummies location and ends it. 
	DestroyDummy();

	// Blends the camera back to the player location and destroys the operator.
	ReturnAndDestroyCameraOperator();

	// Returns the main player character to be visible.
	SetMainActorHidden(false);
}

void APlayerCharacter::AsyncLoadDummy()
{
	// Async loads the dummy player character for use with the combat systems. 
	TWeakObjectPtr<APlayerCharacter> SafeThis(this);
	UAssetManager::GetStreamableManager().RequestAsyncLoad(AIPlayerDummyClass.ToSoftObjectPath(), FStreamableDelegate::CreateLambda([SafeThis]
	{
		if (!SafeThis.IsValid()) return;
		
		if (UClass* LoadedClass = Cast<UClass>(SafeThis->AIPlayerDummyClass.Get()))
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

			if (AActor* SpawnedCombatant = SafeThis->GetWorld()->SpawnActor<AActor>(LoadedClass, SafeThis->GetActorLocation(), SafeThis->GetActorRotation(), SpawnParams))
			{
				SafeThis->PlayerCombatant = Cast<AAI_PlayerCombatant>(SpawnedCombatant);
				SafeThis->AttackController->SetPlayerCombatantReference(SafeThis->PlayerCombatant);
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Player Character: Attempt to load AI player, Loaded Class failure"))
		}
	}));
}

void APlayerCharacter::SetMainActorHidden(const bool SetHidden)
{
	if (SetHidden)
	{
		// This is a tiny delay to ensure that the switch has time to happen without any flashes. 
		FTimerHandle DelayBeforeAction;
		TWeakObjectPtr SafeThis = this;
		GetWorld()->GetTimerManager().SetTimer(DelayBeforeAction, [SafeThis]
		{
			if (SafeThis.IsValid())
			{
				SafeThis->SetActorHiddenInGame(true);
				
			}
		}, 0.1f, false);
	}
	else
	{
		SetActorHiddenInGame(false);
	}
}

void APlayerCharacter::DestroyDummy()
{
	// Places the hidden player in the exact spot as the AI dummy so that it can resume control seamlessly
	if (PlayerCombatant)
	{
		SetActorLocation(PlayerCombatant->GetActorLocation());
		SetActorRotation(PlayerCombatant->GetActorRotation());

		PlayerCombatant->Destroy();
		PlayerCombatant = nullptr;
		AttackController->ClearPlayerCombatantReference();
	}
}

void APlayerCharacter::SpawnAndSetCameraOperator()
{	
	UAssetManager::GetStreamableManager().RequestAsyncLoad(CameraOperatorClass.ToSoftObjectPath(), FStreamableDelegate::CreateLambda([this]
	{
		if (UClass* LoadedClass = Cast<UClass>(CameraOperatorClass.Get()))
		{
			// Adds a little offset to the spawn location
			const FVector SpawnLocation = FVector(GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z + 40.0f);
			CameraOperator = GetWorld()->SpawnActor<ACombatCameraOperator>(LoadedClass, SpawnLocation, GetActorRotation());

			PlayerController->SetViewTargetWithBlend(CameraOperator, 0.5f);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Player Character: Spawn Camera Operator load async failed"))
		}
	}));
}

void APlayerCharacter::ReturnAndDestroyCameraOperator()
{
	// Resets the camera to be the player character
	PlayerController->SetViewTargetWithBlend(this, 1.0f);

	if (!CameraOperator) return;
		
	// This timer is to allow the camera blend time before the destroy actor happens.
	FTimerHandle BlendDelayTimerHandle;
	TWeakObjectPtr<APlayerCharacter> SafeThis = this; // Captures a reference to the player
	TWeakObjectPtr SafeCameraOperator = CameraOperator; // Captures a reference to the camera operator
	GetWorld()->GetTimerManager().SetTimer(BlendDelayTimerHandle, [SafeThis, SafeCameraOperator]()
	{
		// Checks to make sure neither become invalid whilst the timer is running, this would result in a crash if so. 
		if (!SafeThis.IsValid()) return;
		if (SafeCameraOperator.IsValid())
		{
			SafeCameraOperator->Destroy();
		}
	}, 1.5f, false);
}

// Function to send an interface request to check if it is currently the players turn.
bool APlayerCharacter::CheckIsPlayersTurn() const
{
	if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(UGameplayStatics::GetGameMode(GetWorld())))
	{
		return CombatInterface->GetCurrentTurnOrder() == Player;
	}
	return false;
}

// Function to send an interface request to check if the grid slot is currently not occupied or disabled
bool APlayerCharacter::CheckGridSlotAvailable(AActor* BoardPieceActor)
{
	if (BoardPieceActor)
	{
		if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(BoardPieceActor))
		{
			return CombatInterface->GetCurrentPieceState() == EPieceState::Enabled;
		}
		return false;
	}
	UE_LOG(LogTemp, Error, TEXT("Player Character: Check Grid Slot Available - board piece actor reference failed"))
	return false;
}

void APlayerCharacter::SetCombatTurnMode(const EPlayerTurnState NewTurnState)
{
	CurrentPlayerTurnState = NewTurnState;
	
	switch (CurrentPlayerTurnState)
	{
	case EPlayerTurnState::Neutral:
		ExitHoverMode();
		break;
		
	case EPlayerTurnState::MovementMode:
		EnterHoverMode();
		break;
		
	case EPlayerTurnState::CombatMode:
		EnterHoverMode();
		break;
	}
}

// An interface call to either start or end combat for the player
void APlayerCharacter::NotifyCombatStatus(int CombatState)
{
	switch (CombatState)
	{
		case 0:
		UpdatePlayerCombatState(false); // Exit combat
		break;
		case 1:
		UpdatePlayerCombatState(true); // Enter combat
		break;
		
		default:
		UpdatePlayerCombatState(false); // Failsafe exit combat.
	}
}

void APlayerCharacter::MoveAI_Character(const FVector Location)
{
	if (PlayerCombatant)
	{
		// Commands the player AI to move from here for ease of access.
		PlayerCombatant->MoveAI_Character(Location);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Player Character: AI Move Character - AI Actor ref fail"))
	}
}

FIntPoint APlayerCharacter::GetGridCoordinates()
{
	// Gets the AI Dummies grid coordinates for ease of access through the player 
	if (PlayerCombatant)
	{
		return PlayerCombatant->GetGridCoordinates();
	}
	
	return FIntPoint::ZeroValue;
}

void APlayerCharacter::BeginTurnPhase()
{
	// Sends the order to the AI Dummy to execute its turn functionality.
	
	if (InitialTurn)
	{
		FTimerHandle InitialisationDelayHandle;
		TWeakObjectPtr<APlayerCharacter> SafeThis = this;
		
		// If this is on its initial turn it adds a small initialization timer so that it can make sure the AI has spawned before it uses it.
		GetWorld()->GetTimerManager().SetTimer(InitialisationDelayHandle, [SafeThis]
		{
			if (!SafeThis.IsValid())
			{
				if (SafeThis->PlayerCombatant)
				{
					SafeThis->PlayerCombatant->BeginTurnPhase();
				}
			}
		}, 0.5f, false);
		
		InitialTurn = false;
		return;
	}
	
	if (PlayerCombatant)
	{
		PlayerCombatant->BeginTurnPhase();
	}
}

void APlayerCharacter::SetAttackerReference(AActor* AttackerReference)
{
	if (PlayerCombatant)
	{
		PlayerCombatant->SetAttackerReference(AttackerReference);
	}
}

int APlayerCharacter::GetTimePoints()
{
	if (PlayerCombatant)
	{
		return PlayerCombatant->GetTimePoints();
	}
	
	return 0;
}

void APlayerCharacter::SetPlayerHoverMovementModeActive(bool IsActivate)
{
	if (LastTurnStateUsed == EPlayerTurnState::MovementMode)
	{
		SetCombatTurnMode(EPlayerTurnState::MovementMode);
	}
}

void APlayerCharacter::NotifyNewCameraFocus(AActor* Target)
{
	if (!CameraOperator || !Target) return;
	
	OnNewCameraTargetSelected(Target);
}

// Created by Snow Paw Games


#include "PlayerCharacter.h"

#include "Character_Inventory.h"
#include "CombatCameraOperator.h"
#include "EnhancedInputComponent.h"
#include "Engine/AssetManager.h"
#include "GameFramework/CharacterMovementComponent.h"


APlayerCharacter::APlayerCharacter()
{
	CharacterInventory = CreateDefaultSubobject<UCharacter_Inventory>(TEXT("Character Inventory"));
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
	if (PlayerController)
	{
		// Creates an array of types to search for. Only want world dynamic to be triggered
		TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
		ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldDynamic));

		// Checks what the mouse is clicking on, the aim is to detect board pieces only
		FHitResult HitResult;
		PlayerController->GetHitResultUnderCursorForObjects(ObjectTypes, false, HitResult);

		if (HitResult.bBlockingHit)
		{
			// Broadcasts back to blueprints which was hit, where then I can do checks etc...
			OnBoardPieceClicked.Broadcast(HitResult.GetActor());
		}
	}
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
	UAssetManager::GetStreamableManager().RequestAsyncLoad(AIPlayerDummyClass.ToSoftObjectPath(), FStreamableDelegate::CreateLambda([this]
	{
		if (UClass* LoadedClass = Cast<UClass>(AIPlayerDummyClass.Get()))
		{
			AIPlayerDummy = GetWorld()->SpawnActor<AActor>(LoadedClass, GetActorLocation(), GetActorRotation());
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
	// Places the hidden player in the exact spot as the AI dummy so that it can resume control seemlessly
	if (AIPlayerDummy)
	{
		SetActorLocation(AIPlayerDummy->GetActorLocation());
		SetActorRotation(AIPlayerDummy->GetActorRotation());

		AIPlayerDummy->Destroy();
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
	TWeakObjectPtr SafeThis = this; // Captures a reference to the player
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


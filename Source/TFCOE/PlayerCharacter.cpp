// Created by Snow Paw Games


#include "PlayerCharacter.h"

#include "Character_Inventory.h"
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
}

// Initialises the player input system
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (const APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
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
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (CombatEnabled)
		{
			// The combat mode is enabled, the player will no longer be visible and the dummy player will activate
			CombatModeActivated = true;
			SetLocomotion(false, true);
			PlayerController->bShowMouseCursor = true;
			
			UAssetManager::GetStreamableManager().RequestAsyncLoad(CombatPlayer.ToSoftObjectPath(), FStreamableDelegate::CreateLambda([this]
			{
				if (UClass* LoadedClass = Cast<UClass>(CombatPlayer.Get()))
				{
					if (AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(LoadedClass, GetActorLocation(), GetActorRotation()))
					{
						UE_LOG(LogTemp, Error, TEXT("Successfully Spawned AI Actor"))
					}
					else
					{
						UE_LOG(LogTemp, Error, TEXT("Failed to spawn ai actor"))
					}
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("Player Character: Attempt to load AI player, Loaded Class failure"))
				}
			}));
		}
		else
		{
			// Combat is over or not activated, the player will return to visibility and control restored. 
			CombatModeActivated = false;
			SetLocomotion(true, false);
			PlayerController->bShowMouseCursor = false;
		}
	}
}

void APlayerCharacter::MoveTrigger(const FInputActionValue& Value)
{
	const FVector2D MovementVector = Value.Get<FVector2D>();
	
	if (!MovementEnabled)
	{
		ResetMovement();
		return;
	}
	
	// Movement for X vector
	AddMovementInput(FVector(1, 0, 0), MovementVector.X);

	// Movement for Y vector
	AddMovementInput(FVector(0, 1, 0), MovementVector.Y);
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
	if (const APlayerController* PlayerController = Cast<APlayerController>(GetController()))
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

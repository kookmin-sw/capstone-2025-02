// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BSCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/GameplayCameraComponent.h"
#include "Utils/Debug.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
ABSCharacter::ABSCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GameplayCamera = CreateDefaultSubobject<UGameplayCameraComponent>(TEXT("Camera"));
	GameplayCamera->SetupAttachment(GetMesh());
}

// Called when the game starts or when spawned
void ABSCharacter::BeginPlay()
{
	Super::BeginPlay();

	CharacterState = ECharacterState::ECS_Neutral;
	bLockingOn = false;

	ComboCounter = 0;
}

// Called every frame
void ABSCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (LockOnTarget)
	{
		if (AController* MyController = GetController())
		{
			FRotator NewRot = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), LockOnTarget->GetActorLocation());
			MyController->SetControlRotation(NewRot);
		}
	}
}

// Called to bind functionality to input
void ABSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(RollAction, ETriggerEvent::Triggered, this, &ABSCharacter::Roll);
		EnhancedInputComponent->BindAction(LockOnAction, ETriggerEvent::Triggered, this, &ABSCharacter::LockOn);
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &ABSCharacter::Attack);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Triggered, this, &ABSCharacter::Sprint);
	}
}

void ABSCharacter::Roll()
{
	if (CharacterState == ECharacterState::ECS_Sprinting)
		return;

	if (CharacterState != ECharacterState::ECS_Neutral)
	{
		StoreInputToBuffer(RollAction);
		return;
	}

	CharacterState = ECharacterState::ECS_Rolling;

	Debug::LogScreen(TEXT("ROLL START"));

	FVector Direction = GetLastMovementInputVector();
	SetActorRotation(Direction.Rotation());

	PlayRollMontage();
}


void ABSCharacter::PlayRollMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance && RollMontage)
	{
		AnimInstance->Montage_Play(RollMontage);
	}
}

void ABSCharacter::RollEnd()
{
	Debug::LogScreen(TEXT("ROLL END"));

	CharacterState = ECharacterState::ECS_Neutral;

	if (InputBuffer != nullptr)
	{
		ExecuteInputFromBuffer();
	}
}

void ABSCharacter::LockOn()
{
	if (!bLockingOn)
	{
		if (AController* MyController = GetController())
		{
			FVector Start = GetActorLocation();
			FVector End = Start + MyController->GetControlRotation().Vector() * 1000.f;

			FHitResult Result;
			TArray<AActor*> ActorsToIgnore;
			ActorsToIgnore.Add(this);
			TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
			ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

			if (UWorld* World = GetWorld())
			{
				if (UKismetSystemLibrary::SphereTraceSingleForObjects(World, Start, End, 300.f, ObjectTypes, false,
					ActorsToIgnore, EDrawDebugTrace::None, Result, true))
				{
					bLockingOn = true;
					LockOnTarget = Result.GetActor();
				}
			}
		}
	}
	else
	{
		bLockingOn = false;
		LockOnTarget = nullptr;
	}
}

void ABSCharacter::Attack()
{
	if (CharacterState != ECharacterState::ECS_Neutral)
	{
		StoreInputToBuffer(AttackAction);
		return;
	}

	Debug::LogScreen(TEXT("ATTACK START"));

	CharacterState = ECharacterState::ECS_Attacking;

	if (AController* MyController = GetController())
	{
		FVector Direction = MyController->GetControlRotation().Vector();
		SetActorRotation(Direction.Rotation());
	}

	PlayAttackMontage();
}

void ABSCharacter::PlayAttackMontage()
{
	if (AttackMontage)
	{
		Debug::LogScreen(FString::Printf(TEXT("ComboCounter : %d"), ComboCounter));
		PlayMontageBySection(AttackMontage, AttackMontageSections[ComboCounter]);
		ComboCounter++;

		if (ComboCounter >= AttackMontageSections.Num())
			ComboCounter = 0;
	}
}

void ABSCharacter::AttackEnd()
{
	Debug::LogScreen(TEXT("ATTACK END"));

	CharacterState = ECharacterState::ECS_Neutral;

	if (InputBuffer != nullptr)
	{
		if (InputBuffer != AttackAction)
			ComboCounter = 0;

		ExecuteInputFromBuffer();
	}
	else
	{
		ComboCounter = 0;
	}
}

void ABSCharacter::PlayMontageBySection(UAnimMontage* Montage, const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance && Montage)
	{
		AnimInstance->Montage_Play(Montage);
		AnimInstance->Montage_JumpToSection(SectionName, Montage);
	}
}

void ABSCharacter::Sprint(const FInputActionValue& Value)
{
	bool bSprinting = Value.Get<bool>();

	Debug::LogScreen(TEXT("SPRINT"));

	if (bSprinting)
	{
		Debug::LogScreen(TEXT("SPRINT START"));
		CharacterState = ECharacterState::ECS_Sprinting;
	}
	else
	{
		if (CharacterState == ECharacterState::ECS_Sprinting)
		{
			Debug::LogScreen(TEXT("SPRINT END"));
			CharacterState = ECharacterState::ECS_Neutral;
		}
	}
}


void ABSCharacter::StoreInputToBuffer(UInputAction* InputAction)
{
	InputBuffer = InputAction;
	Debug::LogScreen(TEXT("Stored Input To Buffer"));
}

void ABSCharacter::ExecuteInputFromBuffer()
{
	Debug::LogScreen(TEXT("Executing Input From Buffer"));

	CharacterState = ECharacterState::ECS_Neutral;

	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			TArray<UInputModifier*> Modifiers;
			TArray<UInputTrigger*> Triggers;

			if(InputBuffer == AttackAction)
				Debug::LogScreen(TEXT("Is to attack"));
			Subsystem->InjectInputForAction(InputBuffer, FInputActionValue(true), Modifiers, Triggers);
			InputBuffer = nullptr;
		}
	}
}
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
#include "Items/PlayerWeapon.h"

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

	if (UWorld* World = GetWorld())
	{
		if (PlayerWeaponClass)
		{
			PlayerWeapon = World->SpawnActor<APlayerWeapon>(PlayerWeaponClass);
			PlayerWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("WeaponSocket"));
			PlayerWeapon->SetOwner(this);
		}
	}
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

	if (CharacterState == ECharacterState::ECS_Neutral)
	{
		if(InputBuffer != nullptr)
			ExecuteInputFromBuffer();
	}

	//Debug::LogScreen(FString::Printf(TEXT("Curr State : %d"), CharacterState));
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
	if (bIsSprinting)
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
	PrevCharacterState = ECharacterState::ECS_Rolling;
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
	Debug::LogScreen(TEXT("ATTACK"));

	if (CharacterState != ECharacterState::ECS_Neutral)
	{
		StoreInputToBuffer(AttackAction);
		return;
	}

	Debug::LogScreen(TEXT("ATTACK START"));

	CharacterState = ECharacterState::ECS_Attacking;

	if (bLockingOn)
	{
		if (AController* MyController = GetController())
		{
			FRotator NewRot = FRotator(0.f, MyController->GetControlRotation().Yaw, 0.f);
			SetActorRotation(NewRot);
		}
	}

	if (PrevCharacterState == ECharacterState::ECS_Rolling)
	{
		PlayRollAttackMontage();
	}
	else
	{
		if (bIsSprinting)
			PlaySprintAttackMontage();
		else
			PlayComboAttackMontage();
	}
}

void ABSCharacter::AttackEnd()
{
	Debug::LogScreen(TEXT("ATTACK END"));

	CharacterState = ECharacterState::ECS_Neutral;
	PrevCharacterState = ECharacterState::ECS_Attacking;

	if (InputBuffer == AttackAction) // 후속 공격은 여기서 직접 처리
	{
		InputBuffer = nullptr;
		Attack();
	}
	else
	{
		ComboCounter = 0;
	}
}

void ABSCharacter::PlayComboAttackMontage()
{
	Debug::LogScreen(TEXT("COMBO ATTACK MONTAGE"));
	if (ComboAttackMontage)
	{
		Debug::LogScreen(FString::Printf(TEXT("ComboCounter : %d"), ComboCounter));
		PlayMontageBySection(ComboAttackMontage, AttackMontageSections[ComboCounter]);
		ComboCounter++;

		if (ComboCounter >= AttackMontageSections.Num())
			ComboCounter = 0;
	}
}

void ABSCharacter::PlaySprintAttackMontage()
{
	Debug::LogScreen(TEXT("SPRINT ATTACK MONTAGE"));
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && SprintAttackMontage)
	{
		AnimInstance->Montage_Play(SprintAttackMontage);
	}
}

void ABSCharacter::PlayRollAttackMontage()
{
	Debug::LogScreen(TEXT("ROLL ATTACK MONTAGE"));
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && RollAttackMontage)
	{
		AnimInstance->Montage_Play(RollAttackMontage);
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
	bool bToSprint = Value.Get<bool>();

	Debug::LogScreen(TEXT("SPRINT"));

	if (bToSprint)
	{
		Debug::LogScreen(TEXT("SPRINT START"));
		bIsSprinting = true;
	}
	else
	{
		if (bIsSprinting)
		{
			Debug::LogScreen(TEXT("SPRINT END"));
			bIsSprinting = false;
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
	if (!InputBuffer)
		return;

	Debug::LogScreen(TEXT("Executing Input From Buffer"));

	CharacterState = ECharacterState::ECS_Neutral;

	if (APlayerController* PC = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			UEnhancedPlayerInput* PlayerInput = Subsystem->GetPlayerInput();

			FInputActionValue ActionValue(true);
			PlayerInput->InjectInputForAction(InputBuffer, ActionValue);
			Debug::LogScreen(TEXT("Input Injection Finished"));
			InputBuffer = nullptr;
		}
	}	
}
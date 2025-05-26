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
#include "GameFunctionLibrary.h"
#include "Weapons/BSWeapon.h"
#include "Components/BoxComponent.h"
#include "Components/PlayerAttribute.h"
#include "DrawDebugHelpers.h"
#include "UI/BSHUD.h"
#include "UI/BSOverlay.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ABSCharacter::ABSCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CharacterMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh"));
	CharacterMesh->SetupAttachment(GetMesh());

	GameplayCamera = CreateDefaultSubobject<UGameplayCameraComponent>(TEXT("Camera"));
	GameplayCamera->SetupAttachment(GetMesh());

	CharacterMesh->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	CharacterMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CharacterMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	CharacterMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
	CharacterMesh->SetGenerateOverlapEvents(true);

	PotionMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PotionMesh"));
	PotionMesh->SetupAttachment(CharacterMesh);
	PotionMesh->AttachToComponent(CharacterMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, FName("PotionSocket"));
	PotionMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Attribute = CreateDefaultSubobject<UPlayerAttribute>(TEXT("Attributes"));

	LockOnPitch = -15.f;
}

// Called when the game starts or when spawned
void ABSCharacter::BeginPlay()
{
	Super::BeginPlay();

	CharacterState = ECharacterState::ECS_Neutral;

	InputBuffer = nullptr;
	StateBuffer = ECharacterState::ECS_Neutral;

	bLockingOn = false;

	ComboCounter = 0;

	if (UWorld* World = GetWorld())
	{
		if (PlayerWeaponClass)
		{
			PlayerWeapon = World->SpawnActor<ABSWeapon>(PlayerWeaponClass);
			PlayerWeapon->AttachToComponent(CharacterMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, FName("WeaponSocket"));
			PlayerWeapon->SetOwner(this);
			PlayerWeapon->SetInstigator(this);
		}

		TArray<AActor*> AllActors;
		UGameplayStatics::GetAllActorsOfClass(World, ACharacter::StaticClass(), AllActors);
		for (AActor* Actor : AllActors)
		{
			if (Actor != this)
			{
				LockOnTarget = Actor;
				bLockingOn = true;
				break;
			}
		}

		if (PotionMesh)
		{
			PotionMesh->SetVisibility(false);
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
			NewRot.Pitch = LockOnPitch;
			MyController->SetControlRotation(NewRot);
		}
	}
	
	if (CharacterState == ECharacterState::ECS_Neutral && !bIsSprinting)
	{
		if(InputBuffer != nullptr)
			ExecuteInputFromBuffer();

		if (Attribute)
			Attribute->SetRecoverStamina(true);
	}
	else
	{
		if (Attribute)
			Attribute->SetRecoverStamina(false);
	}

	if (bIsSprinting && Attribute)
	{
		Attribute->UseStamina(10.f * DeltaTime);

		if (Attribute->GetCurrentStamina() <= 1.f)
		{
			Sprint(FInputActionValue(false));
		}
	}

	//Debug::LogScreen(FString::Printf(TEXT("%d"), bIsSprinting));
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
		EnhancedInputComponent->BindAction(HealAction, ETriggerEvent::Triggered, this, &ABSCharacter::Heal);
	}
}

void ABSCharacter::GetHit_Implementation(AActor* InAttacker, FVector& ImpactPoint)
{
	if (GetIsInvincible())
	{
		return;
	}
		

	FName SectionName = FName("FromFront");

	FVector Origin;
	FVector BoxExtent;
	GetActorBounds(true, Origin, BoxExtent, false);

	FVector Forward = GetActorForwardVector();
	FVector OriginToImpactPoint = (ImpactPoint - Origin).GetSafeNormal();

	DrawDebugLine(GetWorld(), Origin, ImpactPoint, FColor::Cyan, false, 3.f);

	const float DotResult = FVector::DotProduct(Forward, OriginToImpactPoint);
	float OutAngleDifference = UKismetMathLibrary::DegAcos(DotResult);

	const FVector CrossResult = FVector::CrossProduct(Forward, OriginToImpactPoint);

	if (CrossResult.Z < 0.f)
	{
		OutAngleDifference *= -1.f;
	}

	if (OutAngleDifference >= -45.f && OutAngleDifference <= 45.f)
	{
		SectionName = FName("FromFront");
	}
	else if (OutAngleDifference < -45.f && OutAngleDifference >= -135.f)
	{
		SectionName = FName("FromLeft");
	}
	else if (OutAngleDifference > 45.f && OutAngleDifference <= 135.f)
	{
		SectionName = FName("FromRight");
	}
	else
	{
		SectionName = FName("FromBack");
	}

	if (GEngine)
	{
		FString Msg = FString::Printf(TEXT("SectionName: %s"), *SectionName.ToString());
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, Msg);
	}

	// HitReactMontage
	if (HitReactMontage)
	{
		EnableStun();

		GetMesh()->GetAnimInstance()->OnMontageEnded.AddDynamic(this, &ABSCharacter::DisableStun);
		PlayMontageBySection(HitReactMontage, SectionName);
	}
}

bool ABSCharacter::GetbIsInvincible_Implementation() const
{
	return bIsInvincible;
}

float ABSCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (Attribute)
	{
		if (APlayerController* PC = Cast<APlayerController>(GetController()))
		{
			if (ABSHUD* Hud = Cast<ABSHUD>(PC->GetHUD()))
			{
				if (UBSOverlay* Overlay = Hud->GetBSOverlay())
				{
					
					Attribute->ReceiveDamage(DamageAmount);
					
					
					Overlay->SetHealthBarPercent(Attribute->GetHealthPercent());

					if (Attribute->IsDead())
					{
						// TODO : Dead();
					}
				}
			}
		}

	}
	return 0.0f;
}

void ABSCharacter::Roll()
{
	if (CharacterState != ECharacterState::ECS_Neutral)
	{
		StoreInputToBuffer(RollAction);
		return;
	}

	if (Attribute)
	{
		if (Attribute->GetCurrentStamina() < 10.f)
			return;

		Attribute->UseStamina(10.f);
	}

	CharacterState = ECharacterState::ECS_Rolling;

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
	CharacterState = ECharacterState::ECS_Neutral;
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
		StoreInputToBuffer(AttackAction, true);
		return;
	}

	if (Attribute)
	{
		if (Attribute->GetCurrentStamina() < 20.f)
		{
			ComboCounter = 0;
			StateBuffer = ECharacterState::ECS_Neutral;
			return;
		}
		Attribute->UseStamina(20.f);
	}

	CharacterState = ECharacterState::ECS_Attacking;

	if (AController* MyController = GetController())
	{
		if (bLockingOn)
		{
			FRotator NewRot = FRotator(0.f, MyController->GetControlRotation().Yaw, 0.f);
			SetActorRotation(NewRot);
		}
		else
		{
			if (GetLastMovementInputVector() != FVector::Zero())
			{
				FRotator NewRot = FRotator(0.f, GetLastMovementInputVector().Rotation().Yaw, 0.f);
				SetActorRotation(NewRot);
			}
		}
	}

	if (StateBuffer == ECharacterState::ECS_Rolling)
	{
		PlayRollAttackMontage();
		StateBuffer = ECharacterState::ECS_Neutral;
	}
	else
	{
		if (bIsSprinting)
		{
			PlaySprintAttackMontage();
			Sprint(FInputActionValue(false));
		}
		else
			PlayComboAttackMontage();
	}
}

void ABSCharacter::Heal_Implementation()
{
	if (CharacterState != ECharacterState::ECS_Neutral)
	{
		StoreInputToBuffer(HealAction);
		bCouldHeal = false;
		return;
	}

	if (Attribute->GetHealCount() <= 0)
	{
		bCouldHeal = false;
		return;
	}

	CharacterState = ECharacterState::ECS_Healing;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HealMontage)
	{
		AnimInstance->Montage_Play(HealMontage);
		PotionMesh->SetVisibility(true);
	}
	bCouldHeal = true;
}

void ABSCharacter::HealEnd_Implementation()
{
	Attribute->UseHealItem();
	PotionMesh->SetVisibility(false);

	CharacterState = ECharacterState::ECS_Neutral;
}

void ABSCharacter::AttackEnd()
{
	CharacterState = ECharacterState::ECS_Neutral;

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

	if (ComboAttackMontage)
	{
		PlayMontageBySection(ComboAttackMontage, AttackMontageSections[ComboCounter]);
		ComboCounter++;

		if (ComboCounter >= AttackMontageSections.Num())
			ComboCounter = 0;
	}
}

void ABSCharacter::PlaySprintAttackMontage()
{

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && SprintAttackMontage)
	{
		AnimInstance->Montage_Play(SprintAttackMontage);
	}
}

void ABSCharacter::PlayRollAttackMontage()
{

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

void ABSCharacter::Sprint_Implementation(const FInputActionValue& Value)
{
	bool bToSprint = Value.Get<bool>();

	if (bToSprint)
	{
		bIsSprinting = true;
	}
	else
	{
		if (bIsSprinting)
		{
			bIsSprinting = false;
		}
	}
}


void ABSCharacter::StoreInputToBuffer(UInputAction* InputAction, bool bStoreState)
{
	InputBuffer = InputAction;
	if(bStoreState)
		StateBuffer = CharacterState;
}

void ABSCharacter::ExecuteInputFromBuffer()
{
	if (!InputBuffer)
		return;

	CharacterState = ECharacterState::ECS_Neutral;

	if (APlayerController* PC = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			UEnhancedPlayerInput* PlayerInput = Subsystem->GetPlayerInput();

			FInputActionValue ActionValue(true);
			PlayerInput->InjectInputForAction(InputBuffer, ActionValue);
			InputBuffer = nullptr;
		}
	}	
}

void ABSCharacter::SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled)
{
	if (PlayerWeapon && PlayerWeapon->GetWeaponCollisionBox())
	{
		PlayerWeapon->GetWeaponCollisionBox()->SetCollisionEnabled(CollisionEnabled);
		PlayerWeapon->IgnoreActors.Empty();
	}
}

void ABSCharacter::DisableStun(UAnimMontage* Montage, bool bInterrupted)
{
	Debug::LogScreen(TEXT("DISABLE STUN"));
	GetMesh()->GetAnimInstance()->OnMontageEnded.Clear();

	Controller->SetIgnoreMoveInput(false);
	CharacterState = ECharacterState::ECS_Neutral;
}

void ABSCharacter::EnableStun()
{
	Debug::LogScreen(TEXT("ENABLE STUN"));
	Controller->SetIgnoreMoveInput(true);
	CharacterState = ECharacterState::ECS_Stunned;
}

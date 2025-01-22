// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BSCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/GameplayCameraComponent.h"

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
	}
}

void ABSCharacter::Roll()
{
	if (CharacterState != ECharacterState::ECS_Neutral)
		return;

	CharacterState = ECharacterState::ECS_Rolling;

	FVector MovingDirection = GetVelocity();
	if (MovingDirection.Normalize())
	{
		SetActorRotation(MovingDirection.Rotation());
	}

	PlayRollMontage();
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

					UE_LOG(LogTemp, Warning, TEXT("LOCK ON"));
				}
			}
		}
	}
	else
	{
		bLockingOn = false;
		LockOnTarget = nullptr;

		UE_LOG(LogTemp, Warning, TEXT("LOCK OFF"));

	}
}

void ABSCharacter::PlayRollMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance && RollMontage)
	{
		AnimInstance->Montage_Play(RollMontage);
	}
}




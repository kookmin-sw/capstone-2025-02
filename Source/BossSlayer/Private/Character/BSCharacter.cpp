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
		//TODO
	}
	else
	{
		//TODO
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


// Called every frame
void ABSCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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


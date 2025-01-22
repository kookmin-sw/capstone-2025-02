// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyAnimInstance.h"
#include "Enemy/Enemy.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"


void UEnemyAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	EnemyCharacter = Cast<AEnemy>(TryGetPawnOwner());

	if (EnemyCharacter)
	{
		EnemyMovement = EnemyCharacter->GetCharacterMovement();
	}

}

void UEnemyAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (EnemyMovement)
	{
		GroundSpeed = UKismetMathLibrary::VSizeXY(EnemyMovement->Velocity);

		FRotator BaseRotation = EnemyCharacter->GetActorRotation();
		Direction = CalculateDirection(EnemyMovement->Velocity, BaseRotation);
	}


}

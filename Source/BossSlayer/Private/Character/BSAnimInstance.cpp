// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BSAnimInstance.h"
#include "Character/BSCharacter.h"

void UBSAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	BSCharacter = Cast<ABSCharacter>(TryGetPawnOwner());
	if (BSCharacter)
	{
		BSCharacterMovement = BSCharacter->GetCharacterMovement();
	}
}

void UBSAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

}

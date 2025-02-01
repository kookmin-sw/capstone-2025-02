// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFunctionLibrary.h"
#include "Character/CharacterStates.h"
#include "Kismet/KismetMathLibrary.h"

FName UGameFunctionLibrary::ComputeHitReactDirection(AActor* InAttacker, AActor* InVictim, float& OutAngleDifference)
{
	check(InAttacker && InVictim);

	const FVector VictimForward = InVictim->GetActorForwardVector();
	const FVector VictimToAttackerNormalized = (InAttacker->GetActorLocation() - InVictim->GetActorLocation()).GetSafeNormal();

	const float DotResult = FVector::DotProduct(VictimForward, VictimToAttackerNormalized);
	OutAngleDifference = UKismetMathLibrary::DegAcos(DotResult);

	const FVector CrossResult = FVector::CrossProduct(VictimForward, VictimToAttackerNormalized);

	if (CrossResult.Z < 0.f)
	{
		OutAngleDifference *= -1.f;
	}

	if (OutAngleDifference >= -45.f && OutAngleDifference <= 45.f)
	{
		return FName("FromFront");
	}
	else if (OutAngleDifference < -45.f && OutAngleDifference >= -135.f)
	{
		return FName("FromLeft");
	}
	else if (OutAngleDifference > 45.f && OutAngleDifference <= 135.f)
	{
		return FName("FromRight");
	}
	return FName("FromBack");
}

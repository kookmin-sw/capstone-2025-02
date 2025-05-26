// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/BSPlayerWeapon.h"
#include "Kismet/GameplayStatics.h"

ABSPlayerWeapon::ABSPlayerWeapon()
{

}

void ABSPlayerWeapon::OnSuccessfulHit(AActor* HitActor, const FHitResult& Hit)
{
	const float TimeDilationValue = 0.0001f; 
	const float HitStopDuration = 0.15f;  

	AActor* OwnerActor = GetOwner();

	if (OwnerActor)
	{
		OwnerActor->CustomTimeDilation = TimeDilationValue;
	}

	if (HitActor)
	{
		HitActor->CustomTimeDilation = TimeDilationValue;
	}

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [=]()
		{
			if (OwnerActor)
			{
				OwnerActor->CustomTimeDilation = 1.0f;
			}
			if (HitActor)
			{
				HitActor->CustomTimeDilation = 1.0f;
			}
		}, HitStopDuration, false);

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("Hit Stop!"));
	}
}

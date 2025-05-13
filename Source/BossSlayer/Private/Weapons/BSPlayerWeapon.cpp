// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/BSPlayerWeapon.h"
#include "Kismet/GameplayStatics.h"

ABSPlayerWeapon::ABSPlayerWeapon()
{

}

void ABSPlayerWeapon::OnSuccessfulHit(AActor* HitActor, const FHitResult& Hit)
{
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.1f);

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [=, this]()
		{
			UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.0f);
		}, 0.02f, false);

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Cyan, TEXT("Hit Stop Triggered"));
	}
}

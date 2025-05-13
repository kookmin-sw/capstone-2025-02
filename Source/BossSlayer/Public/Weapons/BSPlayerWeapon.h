// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/BSWeapon.h"
#include "BSPlayerWeapon.generated.h"

/**
 * 
 */
UCLASS()
class BOSSSLAYER_API ABSPlayerWeapon : public ABSWeapon
{
	GENERATED_BODY()
	

public:
	ABSPlayerWeapon();

protected:
	void OnSuccessfulHit(AActor* HitActor, const FHitResult& Hit) override;

};

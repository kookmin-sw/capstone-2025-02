// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BSWeapon.generated.h"

class UBoxComponent;

UCLASS()
class BOSSSLAYER_API ABSWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	ABSWeapon();
	virtual void Tick(float DeltaTime) override;
	
	void AttachMeshToSocket(USceneComponent* InParent, const FName& InSocketName);

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnCollisionBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);



private:
	UPROPERTY(VisibleAnywhere, Category = "Weapons")
	UStaticMeshComponent* WeaponMesh;

	UPROPERTY(VisibleAnywhere, Category = "Weapons")
	UBoxComponent* CollisionBox;
};

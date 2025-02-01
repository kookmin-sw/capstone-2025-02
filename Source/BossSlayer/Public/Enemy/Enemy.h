// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"
#include "Enemy.generated.h"

class UStaticMeshComponent;
class UBoxComponent;
class ABSWeapon;

UCLASS()
class BOSSSLAYER_API AEnemy : public ACharacter, public IHitInterface
{
	GENERATED_BODY()

public:
	AEnemy();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetHit_Implementation(AActor* InAttacker) override;

protected:
	virtual void BeginPlay() override;

	void SpawnAndEquipWeapon();

	UFUNCTION(BlueprintCallable)
	void Attack();

	void PlayHitReactMontage(const FName& SectionName);
	void PlayAttackMontage(const FName& SectionName);

private:
	UPROPERTY(EditAnywhere, Category = "Weapon")
	TSubclassOf<ABSWeapon> WeaponClass;

	ABSWeapon* Weapon;

	/** Montage */
	UPROPERTY(EditAnywhere, Category = "Montages")
	UAnimMontage* AttackMontage;

	UPROPERTY(EditAnywhere, Category = "Montages")
	UAnimMontage* HitReactMontage;


};



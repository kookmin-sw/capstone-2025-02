// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"
#include "Enemy.generated.h"

class UStaticMeshComponent;

UCLASS()
class BOSSSLAYER_API AEnemy : public ACharacter, public IHitInterface
{
	GENERATED_BODY()

public:
	AEnemy();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	virtual void GetHit() override;

protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void Attack();

	void PlayHitReactMontage(const FName& SectionName);
	void PlayAttackMontage(const FName& SectionName);

private:
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* WeaponMesh;

	UPROPERTY(EditAnywhere, Category = "Montages")
	UAnimMontage* AttackMontage;

};



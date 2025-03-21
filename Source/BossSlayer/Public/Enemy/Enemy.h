// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"
#include "Enemy.generated.h"

class UStaticMeshComponent;
class UBoxComponent;
class ABSWeapon;
class UAttributeComponent;
class UBSHealthBarComponent;

UCLASS()
class BOSSSLAYER_API AEnemy : public ACharacter, public IHitInterface
{
	GENERATED_BODY()

public:
	AEnemy();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetHit_Implementation(AActor* InAttacker, FVector& ImpactPoint) override;

	//~ Begin AActor Interface.
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	//~ End AActor Interface

protected:
	virtual void BeginPlay() override;

	//~ Begin APawn Interface.
	virtual void PossessedBy(AController* NewController) override;
	//~ End APawn Interface

	UFUNCTION(BlueprintCallable)
	void SpawnAndEquipWeapon(const FName SocketName);

	UFUNCTION(BlueprintCallable)
	void SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled);

	UFUNCTION(BlueprintCallable)
	void Attack();

	void PlayHitReactMontage(const FName& SectionName);
	void PlayAttackMontage(const FName& SectionName);

private:
	UPROPERTY(VisibleAnywhere)
	UAttributeComponent* AttributeComponent;

	UPROPERTY(VisibleAnywhere)
	UBSHealthBarComponent* HealthBarComponent;

	UPROPERTY(EditAnywhere, Category = "Weapon")
	TSubclassOf<ABSWeapon> WeaponClass;

	ABSWeapon* Weapon;

	/** Montage */
	UPROPERTY(EditAnywhere, Category = "Montages")
	UAnimMontage* AttackMontage;

	UPROPERTY(EditAnywhere, Category = "Montages")
	UAnimMontage* HitReactMontage;


};



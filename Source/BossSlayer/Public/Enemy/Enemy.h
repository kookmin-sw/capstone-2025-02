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
class UMotionWarpingComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAttackEnded);

UCLASS()
class BOSSSLAYER_API AEnemy : public ACharacter, public IHitInterface
{
	GENERATED_BODY()

public:
	AEnemy();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//~ Begin IHitInterface Interface.
	virtual void GetHit_Implementation(AActor* InAttacker, FVector& ImpactPoint) override;
	virtual bool GetbIsInvincible_Implementation() const override;
	//~ End IHitInterface Interface.

	//~ Begin AActor Interface.
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	//~ End AActor Interface

	UPROPERTY(BlueprintAssignable, Category = "Attack")
	FOnAttackEnded OnAttackEnded;

	UFUNCTION(BlueprintCallable)
	void BroadcastAttackEnded();

protected:
	virtual void BeginPlay() override;

	//~ Begin APawn Interface.
	virtual void PossessedBy(AController* NewController) override;
	//~ End APawn Interface


	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly, Category = "Motion Warping")
	UMotionWarpingComponent* MotionWarpingComponent;

	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly, Category = "Attribute")
	UAttributeComponent* AttributeComponent;

	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly, Category = "Health Bar")
	UBSHealthBarComponent* HealthBarComponent;

	UPROPERTY(EditAnywhere, Category = "Weapon")
	TSubclassOf<ABSWeapon> WeaponClass;

	ABSWeapon* Weapon;

	/** Montage */
	UPROPERTY(EditAnywhere, Category = "Montages")
	UAnimMontage* AttackMontage;

	UPROPERTY(EditAnywhere, Category = "Montages")
	UAnimMontage* HitReactMontage;

	UPROPERTY(EditAnywhere, Category = "Montages")
	UAnimMontage* DeathMontage;

	UPROPERTY(BlueprintReadOnly, Category = "State")
	bool bIsDead = false;



	UFUNCTION(BlueprintCallable)
	void SpawnAndEquipWeapon(const FName SocketName);
	

	UFUNCTION(BlueprintCallable)
	void SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled);

	UFUNCTION(BlueprintCallable)
	void Attack();

	void Die();
	

	void PlayHitReactMontage(const FName& SectionName);
	void PlayAttackMontage(const FName& SectionName);

private:
	UPROPERTY()
	UMaterialInstanceDynamic* MeshMaterialInstance;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool bIsInvincible = false;


};



// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Character/CharacterStates.h"
#include "Interfaces/HitInterface.h"
#include "BSCharacter.generated.h"

class UInputAction;
struct FInputActionValue;
class ABSWeapon;
class UPlayerAttribute;
class UStaticMeshComponent;

UCLASS()
class BOSSSLAYER_API ABSCharacter : public ACharacter, public IHitInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABSCharacter();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//~ Begin IHitInterface Interface.
	virtual void GetHit_Implementation(AActor* InAttacker, FVector& ImpactPoint) override;
	virtual bool GetbIsInvincible_Implementation() const override;
	//~ End IHitInterface Interface.

	//~ Begin AActor Interface.
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	//~ End AActor Interface

	FORCEINLINE bool GetIsInvincible() { return bIsInvincible; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/* Input Callback */
	void Roll();
	void LockOn();
	void Attack();
	UFUNCTION(BlueprintNativeEvent)
	void Sprint(const FInputActionValue& Value);
	UFUNCTION(BlueprintNativeEvent)
	void Heal();
	
	/* Play Animation Montage */
	void PlayRollMontage();
	void PlayComboAttackMontage();
	void PlaySprintAttackMontage();
	void PlayRollAttackMontage();
	void PlayMontageBySection(UAnimMontage* Montage, const FName& SectionName);

	/* For Animation Notify*/
	UFUNCTION(BlueprintCallable)
	void RollEnd();
	UFUNCTION(BlueprintCallable)
	void AttackEnd();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void HealEnd();

	/* Input Buffering */
	// bStoreState : 현재 CharacterState도 같이 저장할 것인지 (구르기->공격 등에 사용)
	void StoreInputToBuffer(UInputAction* InputAction, bool bStoreState = false);
	void ExecuteInputFromBuffer();

	UFUNCTION(BlueprintCallable)
	void SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled);

	/* Enable/Disable Movement */
	UFUNCTION()
	void EnableStun();
	UFUNCTION()
	void DisableStun(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	FORCEINLINE void SetIsInvincible(bool Flag) { bIsInvincible = Flag; }

	/* Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UInputAction* RollAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UInputAction* LockOnAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UInputAction* AttackAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UInputAction* SprintAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UInputAction* HealAction;

private:	
	/* Camera */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UGameplayCameraComponent* GameplayCamera;

	// 실제 Kwang의 메시 (GetMesh()는 애니메이션 재생을 위해서만 사용)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* CharacterMesh;

	/*Animation Montage*/
	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* RollMontage;
	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* ComboAttackMontage;
	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* SprintAttackMontage;
	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* RollAttackMontage;
	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* HitReactMontage;
	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* HealMontage;

	UPROPERTY(EditAnywhere, Category = Montages)
	TArray<FName> AttackMontageSections;
	int ComboCounter;

	/* State*/
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	ECharacterState CharacterState;
	

	/* Lock On */
	UPROPERTY()
	AActor* LockOnTarget;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bLockingOn;
	float LockOnPitch;

	/* Input Buffer */
	UPROPERTY()
	UInputAction* InputBuffer;
	ECharacterState StateBuffer;

	/* Sprinting */
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bIsSprinting;

	/* Weapon */
	UPROPERTY()
	ABSWeapon* PlayerWeapon;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<ABSWeapon> PlayerWeaponClass;

	/* Attribute */
	UPROPERTY(VisibleAnywhere)
	UPlayerAttribute* Attribute;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool bIsInvincible;

	/* Heal */
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bCouldHeal;

	UPROPERTY(EditDefaultsOnly)
	UStaticMeshComponent* PotionMesh;
};

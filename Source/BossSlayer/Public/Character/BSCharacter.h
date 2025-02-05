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
	//~ End IHitInterface Interface.

protected:
	/* Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UInputAction* RollAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UInputAction* LockOnAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UInputAction* AttackAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UInputAction* SprintAction;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/* Input Callback */
	void Roll();
	void LockOn();
	void Attack();
	UFUNCTION(BlueprintNativeEvent)
	void Sprint(const FInputActionValue& Value);

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

	/* Input Buffering */
	void StoreInputToBuffer(UInputAction* InputAction);
	void ExecuteInputFromBuffer();

private:	
	/* Component */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UGameplayCameraComponent* GameplayCamera;

	/*Animation Montage*/
	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* RollMontage;
	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* ComboAttackMontage;
	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* SprintAttackMontage;
	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* RollAttackMontage;

	UPROPERTY(EditAnywhere, Category = Montages)
	TArray<FName> AttackMontageSections;
	int ComboCounter;

	/* State*/
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	ECharacterState CharacterState;
	

	/* For Lock On */
	UPROPERTY()
	AActor* LockOnTarget;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bLockingOn;

	/* Input Buffer */
	UPROPERTY()
	UInputAction* InputBuffer;
	ECharacterState StateBuffer;

	/* Sprinting */
	bool bIsSprinting;

	/* Weapon */
	UPROPERTY()
	ABSWeapon* PlayerWeapon;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<ABSWeapon> PlayerWeaponClass;
};

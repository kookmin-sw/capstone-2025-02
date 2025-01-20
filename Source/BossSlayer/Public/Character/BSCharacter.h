// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Character/CharacterStates.h"
#include "BSCharacter.generated.h"

class UInputAction;

UCLASS()
class BOSSSLAYER_API ABSCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABSCharacter();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	/* Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UInputAction* RollAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UInputAction* LockOnAction;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/* Input Callback */
	void Roll();
	void LockOn();

	/* Play Animation Montage */
	void PlayRollMontage();

	/* Animation Notify*/
	UFUNCTION(BlueprintCallable)
	void RollEnd();

private:	
	/* Component */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UGameplayCameraComponent* GameplayCamera;

	/*Animation Montage*/
	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* RollMontage;

	/* State*/
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	ECharacterState CharacterState;


	/* For Lock On */
	UPROPERTY()
	AActor* LockOnTarget;
	bool bLockingOn;
};

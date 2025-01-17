// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BSCharacter.generated.h"

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
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UInputAction* RollAction;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/* Input Callback */
	void Roll();

	/* Play Animation Montage */
	void PlayRollMontage();

	/* Animation Notify*/
	UFUNCTION(BlueprintCallable)
	void RollEnd();

private:	
	/*Animation Montage*/
	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* RollMontage;
};

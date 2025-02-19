// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerAttribute.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BOSSSLAYER_API UPlayerAttribute : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPlayerAttribute();

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void ReceiveDamage(float Damage);
	void UseStamina(float Amount);

	FORCEINLINE void SetRecoverStamina(bool Flag) { bRecoverStamina = Flag; };
	FORCEINLINE float GetCurrentHealth() { return CurrentHealth; }
	FORCEINLINE float GetCurrentStamina() { return CurrentStamina; }
	FORCEINLINE bool IsDead() { return CurrentHealth == 0; }

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:	
	UPROPERTY(EditDefaultsOnly)
	float CurrentStamina;

	UPROPERTY(EditDefaultsOnly)
	float MaxStamina;

	UPROPERTY(EditAnywhere)
	float StaminaRecoverRate;

	UPROPERTY(EditDefaultsOnly)
	float CurrentHealth;

	UPROPERTY(EditDefaultsOnly)
	float MaxHealth;

	bool bRecoverStamina;

	class UBSOverlay* BSOverlay;
};

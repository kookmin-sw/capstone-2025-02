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

	void ChangeHealth(float Amount);
	void UseStamina(float Amount);
	void UseHealItem();
	void ReceiveDamage(float Damage);

	FORCEINLINE void SetRecoverStamina(bool Flag) { bRecoverStamina = Flag; };
	FORCEINLINE float GetCurrentHealth() { return CurrentHealth; }
	FORCEINLINE float GetCurrentStamina() { return CurrentStamina; }
	FORCEINLINE float GetHealCount() { return HealCount; }
	FORCEINLINE float GetHealthPercent() { return CurrentHealth / MaxHealth; }
	FORCEINLINE bool IsDead() { return CurrentHealth == 0; }

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:	
	UPROPERTY()
	class UBSOverlay* BSOverlay;

	/* Health */
	UPROPERTY(EditDefaultsOnly)
	float CurrentHealth;

	UPROPERTY(EditDefaultsOnly)
	float MaxHealth;

	/* Stamina */
	UPROPERTY(EditDefaultsOnly)
	float CurrentStamina;

	UPROPERTY(EditDefaultsOnly)
	float MaxStamina;

	UPROPERTY(EditAnywhere)
	float StaminaRecoverRate;

	bool bRecoverStamina;

	/* Heal */
	UPROPERTY(EditDefaultsOnly)
	int HealCount;

	UPROPERTY(EditDefaultsOnly)
	float HealAmount;
};

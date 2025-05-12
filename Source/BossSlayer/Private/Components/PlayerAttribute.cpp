// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/PlayerAttribute.h"
#include "Kismet/GameplayStatics.h"
#include "UI/BSOverlay.h"
#include "UI/BSHUD.h"
#include "Components/PlayerAttribute.h"

// Sets default values for this component's properties
UPlayerAttribute::UPlayerAttribute()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	bRecoverStamina = false;

	CurrentHealth = 100.f;
	MaxHealth = 100.f;
	CurrentStamina = 100.f;
	MaxStamina = 100.f;
	StaminaRecoverRate = 35.f;

	// ...

	HealCount = 4;
	HealAmount = 60.f;
}


// Called when the game starts
void UPlayerAttribute::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		if (ABSHUD* BSHUD = PC->GetHUD<ABSHUD>())
		{
			BSOverlay = BSHUD->GetBSOverlay();
		}
	}

	BSOverlay->SetHealthBarPercent(CurrentHealth / MaxHealth);
	BSOverlay->SetHealCountText(HealCount);
}


// Called every frame
void UPlayerAttribute::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// 스태미너 자동회복
	if (bRecoverStamina)
	{
		CurrentStamina += DeltaTime * StaminaRecoverRate;
		if (CurrentStamina > MaxStamina)
		{
			CurrentStamina = MaxStamina;
		}
		if (BSOverlay)
			BSOverlay->SetStaminaBarPercent(CurrentStamina / MaxStamina);
	}
}

void UPlayerAttribute::ChangeHealth(float Amount)
{
	CurrentHealth = FMath::Clamp(CurrentHealth + Amount, 0.f, 100.f);

	if (BSOverlay)
		BSOverlay->SetHealthBarPercent(CurrentHealth / MaxHealth);
}

void UPlayerAttribute::UseStamina(float Amount)
{
	CurrentStamina -= Amount;
	if (BSOverlay)
		BSOverlay->SetStaminaBarPercent(CurrentStamina / MaxStamina);
}

void UPlayerAttribute::UseHealItem()
{
	HealCount--;
	if (BSOverlay)
		BSOverlay->SetHealCountText(HealCount);
}

void UPlayerAttribute::ReceiveDamage(float Damage)
{
	CurrentHealth = FMath::Clamp(CurrentHealth - Damage, 0, MaxHealth);
}


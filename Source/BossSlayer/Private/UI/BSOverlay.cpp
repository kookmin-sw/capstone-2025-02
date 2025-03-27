// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/BSOverlay.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UBSOverlay::SetHealthBarPercent(float Percent)
{
	if(HealthBar)
		HealthBar->SetPercent(Percent);
}

void UBSOverlay::SetStaminaBarPercent(float Percent)
{
	if (StaminaBar)
		StaminaBar->SetPercent(Percent);
}

void UBSOverlay::SetHealCountText(int Count)
{
	FString String = FString::FromInt(Count);
	HealCountText->SetText(FText::FromString(String));
}

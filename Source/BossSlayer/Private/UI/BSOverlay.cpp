// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/BSOverlay.h"
#include "Components/ProgressBar.h"

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

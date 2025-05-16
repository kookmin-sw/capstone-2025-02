// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/BSHealthBarComponent.h"
#include "UI/BSHealthBar.h"
#include "Components/ProgressBar.h"

void UBSHealthBarComponent::SetHealthPercent(float percent)
{
	
	if (HealthBarWidget == nullptr)
	{
		HealthBarWidget = Cast<UBSHealthBar>(GetUserWidgetObject());
	}

	if (HealthBarWidget && HealthBarWidget->HealthBar)
	{
		HealthBarWidget->HealthBar->SetPercent(percent);
	}
	
}

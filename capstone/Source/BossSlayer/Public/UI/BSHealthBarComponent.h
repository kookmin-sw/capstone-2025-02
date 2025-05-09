	// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "BSHealthBarComponent.generated.h"

/**
 * 
 */
UCLASS()
class BOSSSLAYER_API UBSHealthBarComponent : public UWidgetComponent
{
	GENERATED_BODY()

public:
	void SetHealthPercent(float percent);

private:
	UPROPERTY()
	class UBSHealthBar* HealthBarWidget;
	
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "BSHUD.generated.h"

class UBSOverlay;

/**
 * 
 */
UCLASS()
class BOSSSLAYER_API ABSHUD : public AHUD
{
	GENERATED_BODY()
	
public:
	FORCEINLINE UBSOverlay* GetBSOverlay() { return BSOverlay; };

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UBSOverlay> BSOverlayClass;

	UPROPERTY()
	UBSOverlay* BSOverlay;
};

// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/BSHUD.h"
#include "UI/BSOverlay.h"

void ABSHUD::BeginPlay()
{
	Super::BeginPlay();

	if (UWorld* World = GetWorld())
	{
		APlayerController* PlayerController = World->GetFirstPlayerController();
		if (PlayerController && BSOverlayClass)
		{
			BSOverlay = CreateWidget<UBSOverlay>(PlayerController, BSOverlayClass);
			BSOverlay->AddToViewport(0);
		}
	}
}

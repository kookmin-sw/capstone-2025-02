// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GenericTeamAgentInterface.h"

#include "BSPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class BOSSSLAYER_API ABSPlayerController : public APlayerController, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	ABSPlayerController();
	
	//~ Begin IGenericTeamAgentInterface Interface//
	virtual FGenericTeamId GetGenericTeamId() const;
	//~ End IGenericTeamAgentInterface Interface//

private:
	FGenericTeamId PlayerTeamId;
};

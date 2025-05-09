// Fill out your copyright notice in the Description page of Project Settings.


#include "Controllers/BSPlayerController.h"

ABSPlayerController::ABSPlayerController()
{
	PlayerTeamId = FGenericTeamId(0);
}

FGenericTeamId ABSPlayerController::GetGenericTeamId() const
{
	return PlayerTeamId;
}
	
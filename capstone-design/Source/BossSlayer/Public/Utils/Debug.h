// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
static class BOSSSLAYER_API Debug
{
public:
	Debug();

	static void LogScreen(const FString& DebugMessage);

	~Debug();
};

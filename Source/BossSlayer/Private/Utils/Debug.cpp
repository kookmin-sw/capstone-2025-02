// Fill out your copyright notice in the Description page of Project Settings.


#include "Utils/Debug.h"

Debug::Debug()
{
}

void Debug::LogScreen(const FString& DebugMessage)
{
	if(GEngine)
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.f, FColor::Cyan, DebugMessage);
}

Debug::~Debug()
{
}

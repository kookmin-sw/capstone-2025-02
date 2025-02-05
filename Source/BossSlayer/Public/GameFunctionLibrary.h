// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameFunctionLibrary.generated.h"



/**
 * 
 */
UCLASS()
class BOSSSLAYER_API UGameFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintPure, Category = "Warrior|FunctionLibrary")
	static FName ComputeHitReactDirection(AActor* InAttacker, AActor* InVictim);
	
};

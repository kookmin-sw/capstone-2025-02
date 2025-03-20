// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BT_Service_OrientToTargetActor.generated.h"

/**
 * 
 */
UCLASS()
class BOSSSLAYER_API UBT_Service_OrientToTargetActor : public UBTService
{
	GENERATED_BODY()

	UBT_Service_OrientToTargetActor();

	//~ Begin UBTNode Interface	
	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;
	virtual FString GetStaticDescription() const override;
	//~ End UBTNode Interface	

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, Category = "Target")
	FBlackboardKeySelector InTargetActorKey;

	UPROPERTY(EditAnywhere, Category = "Target")
	float RotationInterpSpeed;

	
};

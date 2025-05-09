// Fill out your copyright notice in the Description page of Project Settings.


#include "Controllers/BSAiController.h"
#include "Navigation/CrowdFollowingComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Enemy/Enemy.h"

#include "Utils/Debug.h"

ABSAiController::ABSAiController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UCrowdFollowingComponent>("PathFollowingComponent"))
{
	if (UCrowdFollowingComponent* CrowdComp = Cast<UCrowdFollowingComponent>(GetPathFollowingComponent()))
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(1, 5.f, FColor::Blue, TEXT("CrowdComp Valid"));
		}
	}

	AISenseConfig_Sight = CreateDefaultSubobject<UAISenseConfig_Sight>("AISenseConfig_Sight");
	AISenseConfig_Sight->DetectionByAffiliation.bDetectEnemies = true;
	AISenseConfig_Sight->DetectionByAffiliation.bDetectFriendlies = false;
	AISenseConfig_Sight->DetectionByAffiliation.bDetectNeutrals = false;
	AISenseConfig_Sight->SightRadius = 5000.f;
	AISenseConfig_Sight->LoseSightRadius = 0.f;
	AISenseConfig_Sight->PeripheralVisionAngleDegrees = 360.f;

	EnemyPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>("EnemyPerceptionComponent");
	EnemyPerceptionComponent->ConfigureSense(*AISenseConfig_Sight);
	EnemyPerceptionComponent->SetDominantSense(UAISenseConfig_Sight::StaticClass());	
	EnemyPerceptionComponent->OnTargetPerceptionUpdated.AddUniqueDynamic(this, &ThisClass::OnEnemyPerceptionUpdated);

	SetGenericTeamId(1);
}

ETeamAttitude::Type ABSAiController::GetTeamAttitudeTowards(const AActor& Other) const
{
	const APawn* PawnToCheck = Cast<const APawn>(&Other);

	const IGenericTeamAgentInterface* OtherTeamAgent = Cast<IGenericTeamAgentInterface>(PawnToCheck->GetController());
	
	if (OtherTeamAgent && OtherTeamAgent->GetGenericTeamId() < GetGenericTeamId())
	{
		return ETeamAttitude::Hostile;
	}

	return ETeamAttitude::Friendly;
}

void ABSAiController::OnEnemyPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{

	if (UBlackboardComponent* BlackboardComponent = GetBlackboardComponent())
	{
		if (!BlackboardComponent->GetValueAsObject("TargetActor"))
		{
			if (Stimulus.WasSuccessfullySensed() && Actor)
			{
				BlackboardComponent->SetValueAsObject(FName("TargetActor"), Actor);
			}
		}
	}

}

void ABSAiController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (UBlackboardComponent* BlackboardComponent = GetBlackboardComponent())
	{
		AActor* TargetActor = Cast<AActor>(BlackboardComponent->GetValueAsObject(FName("TargetActor")));

		if (TargetActor && GetPawn())
		{
			FVector AILocation = GetPawn()->GetActorLocation();
			FVector PlayerLocation = TargetActor->GetActorLocation();

			// 거리 계산
			float Distance = FVector::Dist(AILocation, PlayerLocation);

			// Blackboard에 거리 값 업데이트
			BlackboardComponent->SetValueAsFloat(FName("TargetDistance"), Distance);
		}
	}
}

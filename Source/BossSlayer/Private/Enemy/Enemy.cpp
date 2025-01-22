


#include "Enemy/Enemy.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimInstance.h"

AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("Weapon Mesh"));
	FAttachmentTransformRules Rules(EAttachmentRule::SnapToTarget, true);
	WeaponMesh->AttachToComponent(GetMesh(), Rules, "RightHandSocket");


}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	
}

void AEnemy::Attack()
{
	// To do
	
	PlayAttackMontage(FName("Attack1"));
}

void AEnemy::PlayHitReactMontage(const FName& SectionName)
{
	// To do
}

void AEnemy::PlayAttackMontage(const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance && AttackMontage)
	{
		AnimInstance->Montage_Play(AttackMontage, 1.5f);
		AnimInstance->Montage_JumpToSection(SectionName, AttackMontage);
	}
}

void AEnemy::Tick(float DeltaTime)
{	
	Super::Tick(DeltaTime);

}

void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemy::GetHit()
{
	// To do
}





#include "Enemy/Enemy.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimInstance.h"
#include "Kismet/KismetMathLibrary.h"
#include "Weapons/BSWeapon.h"
#include "GameFunctionLibrary.h"
#include "Character/CharacterStates.h"
#include "Utils/Debug.h"

AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->MaxWalkSpeed = 300.f;


	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(true);

}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	SpawnAndEquipWeapon();
}

void AEnemy::SpawnAndEquipWeapon()
{

	if (WeaponClass)
	{
		Weapon = GetWorld()->SpawnActor<ABSWeapon>(WeaponClass);

		if (Weapon)
		{
			Weapon->AttachMeshToSocket(GetMesh(), "RightHandSocket");
			Weapon->SetOwner(this);
		}
	}
}

void AEnemy::SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled)
{
	if (Weapon && Weapon->GetWeaponCollisionBox())
	{
		Weapon->GetWeaponCollisionBox()->SetCollisionEnabled(CollisionEnabled);
		Weapon->IgnoreActors.Empty();
	}
}

void AEnemy::Attack()
{
	// To do
	
	PlayAttackMontage(FName("Attack1"));
}

void AEnemy::PlayHitReactMontage(const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance && HitReactMontage)
	{
		AnimInstance->Montage_Play(HitReactMontage);
		AnimInstance->Montage_JumpToSection(SectionName, HitReactMontage);
	}
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


void AEnemy::GetHit_Implementation(AActor* InAttacker, FVector& ImpactPoint)
{
	FName SectionName = UGameFunctionLibrary::ComputeHitReactDirection(InAttacker, this);

	if (GEngine)
	{
		FString Msg = FString::Printf(TEXT("SectionName: %s"), *SectionName.ToString());
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, Msg);
	}

	PlayHitReactMontage(SectionName);
}



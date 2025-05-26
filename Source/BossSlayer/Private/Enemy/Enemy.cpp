	


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
#include "Components/AttributeComponent.h"
#include "Components/WidgetComponent.h"
#include "UI/BSHealthBarComponent.h"
#include "MotionWarpingComponent.h"
#include "Controllers/BSAiController.h"
#include "Components/CapsuleComponent.h"


#include "Utils/Debug.h"

AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = true;

	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->MaxWalkSpeed = 650.f;

	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
	GetMesh()->SetGenerateOverlapEvents(true);

	AttributeComponent = CreateDefaultSubobject<UAttributeComponent>(TEXT("AttributesComponent"));

	HealthBarComponent = CreateDefaultSubobject<UBSHealthBarComponent>(TEXT("HealthBarWidget"));
	HealthBarComponent->SetupAttachment(GetRootComponent());

	MotionWarpingComponent = CreateDefaultSubobject<UMotionWarpingComponent>(TEXT("MotionWarpingComponent"));

	// 0번 슬롯 기준으로 동적 머티리얼 생성
	MeshMaterialInstance = GetMesh()->CreateAndSetMaterialInstanceDynamic(0);


}

void AEnemy::BroadcastAttackEnded()
{
	OnAttackEnded.Broadcast();
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();

}

void AEnemy::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
}

void AEnemy::SpawnAndEquipWeapon(const FName SocketName)
{

	if (WeaponClass)
	{
		Weapon = GetWorld()->SpawnActor<ABSWeapon>(WeaponClass);

		if (Weapon)
		{
			Weapon->AttachMeshToSocket(GetMesh(), SocketName);
			Weapon->SetOwner(this);
			Weapon->SetInstigator(this);
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

void AEnemy::SetHitParticle(UParticleSystem* Particle)
{
	if (Weapon)
	{
		Weapon->SetHitParticle(Particle);
	}
}

void AEnemy::Attack()
{
	// To do
	
	PlayAttackMontage(FName("Attack1"));
}

void AEnemy::Die()
{
	if (bIsDead) return;

	bIsDead = true;

	if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
	{
		if (DeathMontage) 
		{
			float Duration = AnimInstance->Montage_Play(DeathMontage);
			SetLifeSpan(Duration + 0.5f);
		}
	}

	if (ABSAiController* AiController = Cast<ABSAiController>(GetController()))
	{
		AiController->StopMovement();
	}

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
	if (!InAttacker)
	{
		UE_LOG(LogTemp, Warning, TEXT("GetHit: InAttacker is null!"));
		return;
	}

	FName SectionName = UGameFunctionLibrary::ComputeHitReactDirection(InAttacker, this);

	if (GEngine)
	{
		FString Msg = FString::Printf(TEXT("SectionName: %s"), *SectionName.ToString());
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, Msg);
	}

	PlayHitReactMontage(SectionName);
}

bool AEnemy::GetbIsInvincible_Implementation() const
{
	return bIsInvincible;
}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (AttributeComponent && HealthBarComponent)
	{
		AttributeComponent->ReceiveDamage(DamageAmount);

		if (AttributeComponent->IsDead())
		{
			Die();
		}

		HealthBarComponent->SetHealthPercent(AttributeComponent->GetHealthPercent());
		
	}
	return 0.0f;
}



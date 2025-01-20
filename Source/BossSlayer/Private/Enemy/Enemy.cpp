


#include "Enemy/Enemy.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"

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

void AEnemy::Tick(float DeltaTime)
{	
	Super::Tick(DeltaTime);

}

void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}


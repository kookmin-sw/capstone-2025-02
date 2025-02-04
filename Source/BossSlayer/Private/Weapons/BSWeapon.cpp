

#include "Weapons/BSWeapon.h"
#include "Components/BoxComponent.h"
#include "Interfaces/HitInterface.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/SceneComponent.h"


ABSWeapon::ABSWeapon()			
{
	PrimaryActorTick.bCanEverTick = true;

	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	SetRootComponent(WeaponMesh);

	WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	WeaponCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	WeaponCollisionBox->SetupAttachment(GetRootComponent());
	WeaponCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponCollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	WeaponCollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	WeaponCollisionBox->OnComponentBeginOverlap.AddDynamic(this, &ABSWeapon::OnCollisionBoxBeginOverlap);

	BoxTraceStart = CreateDefaultSubobject<USceneComponent>(TEXT("BoxTraceStart"));
	BoxTraceStart->SetupAttachment(GetRootComponent());

	BoxTraceEnd = CreateDefaultSubobject<USceneComponent>(TEXT("BoxTraceEnd"));
	BoxTraceEnd->SetupAttachment(GetRootComponent());

}


void ABSWeapon::BeginPlay()
{
	Super::BeginPlay();

	WeaponCollisionBox->OnComponentBeginOverlap.AddDynamic(this, &ABSWeapon::OnCollisionBoxBeginOverlap);

}

void ABSWeapon::OnCollisionBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == GetOwner())
		return;

	FHitResult BoxHit;
	BoxTrace(BoxHit);

	if (BoxHit.GetActor())
	{
		if (IHitInterface* HitInterface = Cast<IHitInterface>(OtherActor))
		{
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(1, 5.f, FColor::Red, TEXT("Success Hit"));
			}
			HitInterface->Execute_GetHit(BoxHit.GetActor(), GetOwner(), BoxHit.ImpactPoint);

		}
	}
}

void ABSWeapon::BoxTrace(FHitResult& BoxHit)
{
	const FVector Start = BoxTraceStart->GetComponentLocation();
	const FVector End = BoxTraceEnd->GetComponentLocation();

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);
	ActorsToIgnore.Add(GetOwner());

	for (AActor* Actor : IgnoreActors)
	{
		ActorsToIgnore.AddUnique(Actor);
	}

	UKismetSystemLibrary::BoxTraceSingle(
		this,
		Start,
		End,
		FVector(15.f, 15.f, 15.f),
		BoxTraceStart->GetComponentRotation(),
		ETraceTypeQuery::TraceTypeQuery1,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::ForDuration,
		BoxHit,
		true
	);
	IgnoreActors.AddUnique(BoxHit.GetActor());
}


	

void ABSWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABSWeapon::AttachMeshToSocket(USceneComponent* InParent, const FName& InSocketName)
{
	FAttachmentTransformRules Rules(EAttachmentRule::SnapToTarget, true);
	AttachToComponent(InParent, Rules, InSocketName);
}


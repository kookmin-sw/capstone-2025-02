

#include "Weapons/BSWeapon.h"
#include "Components/BoxComponent.h"
#include "Interfaces/HitInterface.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/SceneComponent.h"
#include "Kismet/GameplayStatics.h"


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

	if (IgnoreActors.Contains(OtherActor))
		return;

	FHitResult BoxHit;
	BoxTrace(BoxHit);

	if (BoxHit.GetActor())
	{
		if (IHitInterface* HitInterface = Cast<IHitInterface>(BoxHit.GetActor()))
		{

			if (HitInterface->Execute_GetbIsInvincible(BoxHit.GetActor()))
			{
				return;
			}

			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(1, 5.f, FColor::Red, TEXT("Success Hit"));
			}

			UE_LOG(LogTemp, Warning, TEXT("BoxHit Actor: %s"), *BoxHit.GetActor()->GetName());

			HitInterface->Execute_GetHit(BoxHit.GetActor(), GetOwner(), BoxHit.ImpactPoint);


			if (HitParticle)
			{
				UGameplayStatics::SpawnEmitterAtLocation(
					GetWorld(),
					HitParticle,
					BoxHit.GetActor()->GetActorLocation(),
					FRotator::ZeroRotator
				);
			}

			OnSuccessfulHit(BoxHit.GetActor(), BoxHit);

		}

		UGameplayStatics::ApplyDamage(
			BoxHit.GetActor(),
			Damage,
			GetInstigator()->GetController(),
			GetOwner(),
			UDamageType::StaticClass()
		);
	}
}

void ABSWeapon::OnSuccessfulHit(AActor* HitActor, const FHitResult& Hit)
{

}

void ABSWeapon::BoxTrace(FHitResult& BoxHit)
{
	const FVector Start = BoxTraceStart->GetComponentLocation();
	const FVector End = BoxTraceEnd->GetComponentLocation();

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.AddUnique(this);
	ActorsToIgnore.AddUnique(GetOwner());

	for (AActor* Actor : IgnoreActors)
	{
		ActorsToIgnore.AddUnique(Actor);
	}

	UKismetSystemLibrary::BoxTraceSingle(
		this,
		Start,
		End,
		FVector(30.f, 30.f, 30.f),
		BoxTraceStart->GetComponentRotation(),
		ETraceTypeQuery::TraceTypeQuery1,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::None,
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


// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Weapons/Weapon.h"

#include "NiagaraComponent.h"
#include "Characters/MainCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Interfaces/HitInterface.h"

AWeapon::AWeapon()
{
	WeaponBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Weapon Box"));
	WeaponBox->SetupAttachment(GetRootComponent());
	WeaponBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	WeaponBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

	TraceStartComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Trace Start Component"));
	TraceStartComponent->SetupAttachment(GetRootComponent());

	TraceEndComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Trace End Component"));
	TraceEndComponent->SetupAttachment(GetRootComponent());
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	WeaponBox->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnWeaponBoxOverlap);
	WeaponBox->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnWeaponBoxEndOverlap);
}

void AWeapon::Equip(USceneComponent* InParent, FName InSocketName, AActor* NewOwner, APawn* NewInstigator)
{
	ItemState = EItemState::EIS_Equipped;
	SetOwner(NewOwner);
	SetInstigator(NewInstigator);
	AttachMeshToSocket(InParent, InSocketName);
	DisableAreaSphereCollision();

	PlayEquipSound();
	DeactivateEmbers();
}

void AWeapon::PlayEquipSound()
{
	if (!EquipSound) return;

	UGameplayStatics::PlaySoundAtLocation(this, EquipSound, GetActorLocation());
}

void AWeapon::DisableAreaSphereCollision()
{
	if (!AreaSphere) return;

	AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AWeapon::DeactivateEmbers()
{
	if (!ItemEffect) return;

	ItemEffect->Deactivate();
}

void AWeapon::AttachMeshToSocket(USceneComponent* InParent, const FName& InSocketName)
{
	const FAttachmentTransformRules TransformRules(EAttachmentRule::SnapToTarget, true);
	ItemMesh->AttachToComponent(InParent, TransformRules, InSocketName);
}

void AWeapon::WeaponBoxTrace(FHitResult& WeaponBoxHit)
{
	const FVector Start = TraceStartComponent->GetComponentLocation();
	const FVector End = TraceEndComponent->GetComponentLocation();

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this); // Ignore itself

	for (AActor* Actor : IgnoreActors)
	{
		ActorsToIgnore.AddUnique(Actor);
	}
	
	// Box Trace Single will basically trace for visibility. This is why we put any ETraceTypeQuery
	UKismetSystemLibrary::BoxTraceSingle(
		this,
		Start,
		End,
		WeaponBoxTraceExtent,
		TraceStartComponent->GetComponentRotation(),
		ETraceTypeQuery::TraceTypeQuery1,
		false,
		ActorsToIgnore,
		bShowWeaponBoxDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,
		WeaponBoxHit,
		true // Ignore itself
	);

	IgnoreActors.AddUnique(WeaponBoxHit.GetActor());
}

void AWeapon::ExecuteGetHit(FHitResult& WeaponBoxHit)
{
	IHitInterface* HitInterface = Cast<IHitInterface>(WeaponBoxHit.GetActor());
	// This make sure that our actor implements the interface we're checking
	if (HitInterface)
	{
		// HitInterface->GetHit(WeaponBoxHit.ImpactPoint);
		HitInterface->Execute_GetHit(WeaponBoxHit.GetActor(), WeaponBoxHit.ImpactPoint, GetOwner());
	}
}

bool AWeapon::ActorIsSameType(AActor* OtherActor)
{
	if (!OtherActor) return false;
	
	return GetOwner()->ActorHasTag(FName("Enemy")) && OtherActor->ActorHasTag(FName("Enemy"));
}

void AWeapon::OnWeaponBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ActorIsSameType(OtherActor)) return;
	
	FHitResult WeaponBoxHit;
	WeaponBoxTrace(WeaponBoxHit);
	
	if (WeaponBoxHit.GetActor())
	{
		if (ActorIsSameType(WeaponBoxHit.GetActor())) return;
		
		UGameplayStatics::ApplyDamage(WeaponBoxHit.GetActor(), Damage, GetInstigator()->GetController(), this, UDamageType::StaticClass());
		ExecuteGetHit(WeaponBoxHit);
		CreateFields(WeaponBoxHit.ImpactPoint);
	}
}

void AWeapon::OnWeaponBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}

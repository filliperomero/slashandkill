// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Enemy.h"

#include "Components/AttributeComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "HUD/HealthBarComponent.h"
#include "Kismet/GameplayStatics.h"
#include "SlashAndKill/DebugMacros.h"
// #include "Kismet/KismetSystemLibrary.h"

AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	if (GetMesh())
	{
		GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
		GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
		GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
		GetMesh()->SetGenerateOverlapEvents(true);
	}

	if (GetCapsuleComponent())
	{
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	}

	Attributes = CreateDefaultSubobject<UAttributeComponent>(TEXT("Attributes"));
	HealthBarWidget = CreateDefaultSubobject<UHealthBarComponent>(TEXT("HealthBar"));
	HealthBarWidget->SetupAttachment(GetRootComponent());
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

void AEnemy::GetHit_Implementation(const FVector& ImpactPoint)
{
	if (Attributes && Attributes->IsAlive())
	{
		DirectionalHitReact(ImpactPoint);
	}
	else
	{
		Die();
	}
	
	if (HitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, HitSound, ImpactPoint);
	}

	if (HitParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(this, HitParticle, ImpactPoint);
	}
}

void AEnemy::Die()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance == nullptr || DeathMontage == nullptr) return;
	
	AnimInstance->Montage_Play(DeathMontage);
	const int32 Selection = FMath::RandRange(0, 5);
	FName SectionName = FName();

	switch(Selection)
	{
	case 0:
		SectionName = FName("Death1");
		DeathPose = EDeathPose::EDP_Death01;
		break;
	case 1:
		SectionName = FName("Death2");
		DeathPose = EDeathPose::EDP_Death02;
		break;
	case 2:
		SectionName = FName("Death3");
		DeathPose = EDeathPose::EDP_Death03;
		break;
	case 3:
		SectionName = FName("Death4");
		DeathPose = EDeathPose::EDP_Death04;
		break;
	case 4:
		SectionName = FName("Death5");
		DeathPose = EDeathPose::EDP_Death05;
		break;
	case 5:
		SectionName = FName("Death6");
		DeathPose = EDeathPose::EDP_Death06;
		break;
	default:
		SectionName = FName("Death1");
		DeathPose = EDeathPose::EDP_Death01;
	}
	AnimInstance->Montage_JumpToSection(SectionName, DeathMontage);
}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	if (Attributes && HealthBarWidget)
	{
		Attributes->ReceiveDamage(DamageAmount);
		HealthBarWidget->SetHealthPercent(Attributes->GetHealthPercent());
	}

	return DamageAmount;
}

void AEnemy::DirectionalHitReact(const FVector& ImpactPoint)
{
	const FVector Forward = GetActorForwardVector();
	// Lower Impact Point to the Enemy's Actor Location Z
	const FVector ImpactLowered(ImpactPoint.X, ImpactPoint.Y, GetActorLocation().Z);
	// GetSafeNormal will get the result Vector and normalize it
	const FVector ToHit = (ImpactLowered - GetActorLocation()).GetSafeNormal();
	
	/**
	 * Forward * ToHit = |Forward| |ToHit| * cos(theta)
	 * |Forward| = 1, |ToHit| = 1, so Forward * ToHit = cos(theta)
	 */
	const double CosTheta = FVector::DotProduct(Forward, ToHit);
	// Take the inverse cosine (arc-cosine) of cos(theta) to get theta
	double Theta = FMath::Acos(CosTheta);
	// Convert from radians to degrees
	Theta = FMath::RadiansToDegrees(Theta);

	// If CrossProduct points down, Theta should be negative
	const FVector CrossProduct = FVector::CrossProduct(Forward, ToHit);

	if (CrossProduct.Z < 0)	Theta *= -1.f;

	// Debug Functions
	// UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(), GetActorLocation() + Forward * 60.f, 5.f, FColor::Red, 5.f);
	// UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(), GetActorLocation() + ToHit * 60.f, 5.f, FColor::Green, 5.f);
	// UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(), GetActorLocation() + CrossProduct * 100.f, 5.f, FColor::Orange, 5.f);
	// DRAW_SPHERE_COLOUR(ImpactPoint, FColor::Orange);
	
	FName SectionName("FromBack");

	if (Theta >= -45.f && Theta < 45.f)
	{
		SectionName = FName("FromFront");
	}
	else if (Theta >= -135.f && Theta < -45.f)
	{
		SectionName = FName("FromLeft");
	}
	else if (Theta >= 45.f && Theta < 135.f)
	{
		SectionName = FName("FromRight");
	}
	
	PlayHitReactMontage(SectionName);
}

void AEnemy::PlayHitReactMontage(const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance == nullptr || HitReactMontage == nullptr) return;
	
	AnimInstance->Montage_Play(HitReactMontage);
	AnimInstance->Montage_JumpToSection(SectionName, HitReactMontage);
}

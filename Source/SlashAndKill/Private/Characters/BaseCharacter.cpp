// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/BaseCharacter.h"
#include "Components/BoxComponent.h"
#include "Items/Weapons/Weapon.h"
#include "Components/AttributeComponent.h"

ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	Attributes = CreateDefaultSubobject<UAttributeComponent>(TEXT("Attributes"));
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABaseCharacter::Attack()
{
}

void ABaseCharacter::Die()
{
}

void ABaseCharacter::DirectionalHitReact(const FVector& ImpactPoint)
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

bool ABaseCharacter::CanAttack() const
{
	return false;
}

void ABaseCharacter::AttackEnd()
{
}

void ABaseCharacter::PlayAttackMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance == nullptr || AttackMontage == nullptr) return;
	
	AnimInstance->Montage_Play(AttackMontage);
	const int32 Selection = FMath::RandRange(0, AttackMontage->CompositeSections.Num() - 1);
	const FName SectionName = AttackMontage->GetSectionName(Selection);
	AnimInstance->Montage_JumpToSection(SectionName, AttackMontage);
}

void ABaseCharacter::PlayHitReactMontage(const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance == nullptr || HitReactMontage == nullptr) return;
	
	AnimInstance->Montage_Play(HitReactMontage);
	AnimInstance->Montage_JumpToSection(SectionName, HitReactMontage);
}

void ABaseCharacter::SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled)
{
	if (EquippedWeapon == nullptr || EquippedWeapon->GetWeaponBox() == nullptr) return;
	
	EquippedWeapon->GetWeaponBox()->SetCollisionEnabled(CollisionEnabled);
	EquippedWeapon->IgnoreActors.Empty();
}

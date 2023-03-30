// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"
#include "Characters/CharacterTypes.h"
#include "BaseCharacter.generated.h"

class AWeapon;
class UAttributeComponent;
class UAnimMontage;

UCLASS()
class SLASHANDKILL_API ABaseCharacter : public ACharacter, public IHitInterface
{
	GENERATED_BODY()

public:
	ABaseCharacter();
	virtual void Tick(float DeltaTime) override;
	
protected:
	virtual void BeginPlay() override;
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;
	virtual void Attack();
	virtual void Die();
	virtual void DirectionalHitReact(const FVector& ImpactPoint);
	virtual bool IsAlive();
	virtual void HandleDamage(float DamageAmount);
	void PlayHitSound(const FVector& ImpactPoint);
	void SpawnHitParticles(const FVector& ImpactPoint);
	void DisableCapsule();
	virtual bool CanAttack();

	/** Play montage functions */
	virtual void PlayMontageSection(UAnimMontage* Montage, const FName& SectionName);
	int32 PlayRandomMontageSection(UAnimMontage* Montage);
	virtual void PlayHitReactMontage(const FName& SectionName);
	virtual int32 PlayAttackMontage();
	void StopAttackMontage();
	virtual int32 PlayDeathMontage();
	virtual void PlayDodgeMontage();

	UFUNCTION(BlueprintCallable)
	FVector GetTranslationWarpTarget();

	UFUNCTION(BlueprintCallable)
	FVector GetRotationWarpTarget();
    
    UFUNCTION(BlueprintCallable)
    virtual void AttackEnd();

	UFUNCTION(BlueprintCallable)
	virtual void DodgeEnd();
	
	UFUNCTION(BlueprintCallable)
	void SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled);

	UPROPERTY(VisibleAnywhere, Category = "Weapon")
	TObjectPtr<AWeapon> EquippedWeapon;

	UPROPERTY(BlueprintReadOnly, Category = Combat)
	TObjectPtr<AActor> CombatTarget;

	UPROPERTY(EditAnywhere, Category = Combat)
	double WarpTargetDistance = 75.f;
	
	/**
	 * Components
	 */
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAttributeComponent> Attributes;

	UPROPERTY(BlueprintReadOnly)
	TEnumAsByte<EDeathPose> DeathPose;

private:
	UPROPERTY(EditDefaultsOnly, Category = Montages)
	TObjectPtr<UAnimMontage> AttackMontage;

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	TObjectPtr<UAnimMontage> HitReactMontage;

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	TObjectPtr<UAnimMontage> DeathMontage;

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	TObjectPtr<UAnimMontage> DodgeMontage;
	
	UPROPERTY(EditAnywhere, Category = Sounds)
	TObjectPtr<USoundBase> HitSound;

	UPROPERTY(EditAnywhere, Category = VFX)
	TObjectPtr<UParticleSystem> HitParticle;

public:
	FORCEINLINE TEnumAsByte<EDeathPose> GetDeathPose() const { return DeathPose; }
	
};

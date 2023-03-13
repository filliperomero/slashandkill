// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"
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
	virtual void Attack();
	virtual void Die();
	virtual void DirectionalHitReact(const FVector& ImpactPoint);
	virtual bool IsAlive();
	virtual void HandleDamage(float DamageAmount);
	void PlayHitSound(const FVector& ImpactPoint);
	void SpawnHitParticles(const FVector& ImpactPoint);
	void DisableCapsule();
	virtual bool CanAttack();

	/**
	 * Play montage functions
	 */
	
	virtual void PlayMontageSection(UAnimMontage* Montage, const FName& SectionName);
	int32 PlayRandomMontageSection(UAnimMontage* Montage);
	virtual void PlayHitReactMontage(const FName& SectionName);
	virtual int32 PlayAttackMontage();
	virtual int32 PlayDeathMontage();
    
    UFUNCTION(BlueprintCallable)
    virtual void AttackEnd();
	
	UFUNCTION(BlueprintCallable)
	void SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled);

	UPROPERTY(VisibleAnywhere, Category = "Weapon")
	TObjectPtr<AWeapon> EquippedWeapon;
	
	/**
	 * Components
	 */
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAttributeComponent> Attributes;

private:
	UPROPERTY(EditDefaultsOnly, Category = Montages)
	TObjectPtr<UAnimMontage> AttackMontage;

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	TObjectPtr<UAnimMontage> HitReactMontage;

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	TObjectPtr<UAnimMontage> DeathMontage;
	
	UPROPERTY(EditAnywhere, Category = Sounds)
	TObjectPtr<USoundBase> HitSound;

	UPROPERTY(EditAnywhere, Category = VFX)
	TObjectPtr<UParticleSystem> HitParticle;
	
};

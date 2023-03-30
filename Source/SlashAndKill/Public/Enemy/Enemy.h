// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/BaseCharacter.h"
#include "Characters/CharacterTypes.h"
#include "Enemy.generated.h"

class AAIController;
class UHealthBarComponent;
class UPawnSensingComponent;
class AWeapon;
class ASoul;

UCLASS()
class SLASHANDKILL_API AEnemy : public ABaseCharacter
{
	GENERATED_BODY()

public:
	AEnemy();
	virtual void Tick(float DeltaTime) override;
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	virtual void Destroyed() override;
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;
	
protected:
	virtual void BeginPlay() override;
	
	virtual void Die() override;
	void SpawnSoul();
	virtual void Attack() override;
	virtual bool CanAttack() override;
	virtual void AttackEnd() override;
	virtual void HandleDamage(float DamageAmount) override;

	UPROPERTY(BlueprintReadOnly)
	EEnemyState EnemyState = EEnemyState::EES_Patrolling;

private:	
	/**
	 * AI Behavior
	 */

	void InitializeEnemy();
	void CheckCombatTarget();
	void CheckPatrolTarget();
	void PatrolTimerFinished();
	void StartAttackTimer();
	void ClearAttackTimer();
	bool InTargetRange(AActor* Target, double Radius);
	void MoveToTarget(AActor* Target);
	AActor* ChoosePatrolTarget();
	void ToggleHealthBar(const bool bShow) const;
	void LoseInterest();
	void StartPatrolling();
	void ChaseTarget();
	void ClearPatrolTimer();
	void SpawnDefaultWeapon();
	bool IsOutsideCombatRadius();
	bool IsOutsideAttackRadius();
	bool IsInsideAttackRadius();
	bool IsChasing();
	bool IsAttacking();
	bool IsEngaged();
	bool IsDead();

	UFUNCTION()
	void PawnSeen(APawn* SeenPawn); // Callback for OnPawnSeen in UPawnSensingComponent

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UHealthBarComponent> HealthBarWidget;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPawnSensingComponent> PawnSensing;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AWeapon> WeaponClass;

	UPROPERTY(EditAnywhere)
	double CombatRadius = 1000.f;

	UPROPERTY(EditAnywhere)
	double AttackRadius = 200.f;
	
	UPROPERTY()
	TObjectPtr<AAIController> EnemyController;

	// Current Patrol Target
	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	TObjectPtr<AActor> PatrolTarget;

	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	TArray<TObjectPtr<AActor>> PatrolTargets;

	UPROPERTY(EditAnywhere)
	double PatrolRadius = 200.f;

	FTimerHandle PatrolTimer;
	
	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float PatrolWaitMin = 5.f;

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float PatrolWaitMax = 10.f;

	UPROPERTY(EditAnywhere)
	float PatrollingSpeed = 125.f;
	
	FTimerHandle AttackTimer;

	UPROPERTY(EditAnywhere, Category = Combat)
	float AttackMin = 0.5f;

	UPROPERTY(EditAnywhere, Category = Combat)
	float AttackMax = 1.f;

	UPROPERTY(EditAnywhere)
	float ChasingSpeed = 300.f;
	
	UPROPERTY(EditAnywhere, Category = Combat)
	float DeathLifeSpan = 8.f;

	UPROPERTY(EditAnywhere, Category = Combat)
	TSubclassOf<ASoul> SoulClass;
	
};

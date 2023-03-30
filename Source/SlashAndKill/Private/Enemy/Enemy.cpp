// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Enemy.h"
#include "AIController.h"
#include "Components/AttributeComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Perception/PawnSensingComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "HUD/HealthBarComponent.h"
#include "Items/Soul.h"
#include "Items/Weapons/Weapon.h"

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
		
	HealthBarWidget = CreateDefaultSubobject<UHealthBarComponent>(TEXT("HealthBar"));
	HealthBarWidget->SetupAttachment(GetRootComponent());

	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	PawnSensing = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensing"));
	PawnSensing->SetPeripheralVisionAngle(45.f);
	PawnSensing->SightRadius = 4000.f;
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	if (PawnSensing) PawnSensing->OnSeePawn.AddDynamic(this, &ThisClass::PawnSeen);
	
	InitializeEnemy();

	Tags.Add(FName("Enemy"));
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsDead()) return;

	if (EnemyState > EEnemyState::EES_Patrolling)
	{
		CheckCombatTarget();
	}
	else
	{
		CheckPatrolTarget();
	}
}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
                         AActor* DamageCauser)
{
	CombatTarget = EventInstigator->GetPawn();
	HandleDamage(DamageAmount);
	
	if (IsInsideAttackRadius())
	{
		EnemyState = EEnemyState::EES_Attacking;
	}
	else if (IsOutsideAttackRadius())
	{
		ChaseTarget();
	}

	return DamageAmount;
}

void AEnemy::Destroyed()
{
	Super::Destroyed();

	if (EquippedWeapon)
	{
		EquippedWeapon->Destroy();
	}
}

void AEnemy::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	Super::GetHit_Implementation(ImpactPoint, Hitter);
	
	if (!IsDead()) ToggleHealthBar(true);
	
	SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);
	ClearPatrolTimer();
	ClearAttackTimer();
	StopAttackMontage();
}

void AEnemy::Die()
{
	Super::Die();
	
	EnemyState = EEnemyState::EES_Dead;
	ClearAttackTimer();
	ToggleHealthBar(false);
	DisableCapsule();
	SetLifeSpan(DeathLifeSpan);
	GetCharacterMovement()->bOrientRotationToMovement = false;
	SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);

	SpawnSoul();
}

void AEnemy::SpawnSoul()
{
	UWorld* World = GetWorld();
	
	if (SoulClass && World && Attributes)
	{
		ASoul* SpawnedSoul = World->SpawnActor<ASoul>(SoulClass, GetActorLocation(), GetActorRotation());
		if (SpawnedSoul) SpawnedSoul->SetSouls(Attributes->GetSouls());
	}
}

void AEnemy::Attack()
{
	Super::Attack();
	if (CombatTarget == nullptr) return;
	
	EnemyState = EEnemyState::EES_Engaged;

	PlayAttackMontage();
}

bool AEnemy::CanAttack()
{
	return IsInsideAttackRadius() && !IsAttacking() && !IsDead() && !IsEngaged();
}

void AEnemy::AttackEnd()
{
	Super::AttackEnd();

	EnemyState = EEnemyState::EES_NoState;
	CheckCombatTarget();
}

void AEnemy::HandleDamage(float DamageAmount)
{
	Super::HandleDamage(DamageAmount);
	if (HealthBarWidget)
	{
		HealthBarWidget->SetHealthPercent(Attributes->GetHealthPercent());
	}
}

void AEnemy::InitializeEnemy()
{
	EnemyController = Cast<AAIController>(GetController());
	MoveToTarget(PatrolTarget);
	ToggleHealthBar(false);
	SpawnDefaultWeapon();
}

void AEnemy::CheckCombatTarget()
{
	if (IsOutsideCombatRadius())
	{
		ClearAttackTimer();
		LoseInterest();
		if (!IsEngaged()) StartPatrolling();
	}
	else if (IsOutsideAttackRadius() && !IsChasing())
	{
		ClearAttackTimer();
		if (!IsEngaged()) ChaseTarget();
	}
	else if (CanAttack())
	{
		StartAttackTimer();
	}
}

void AEnemy::CheckPatrolTarget()
{
	if (InTargetRange(PatrolTarget, PatrolRadius))
	{
		PatrolTarget = ChoosePatrolTarget();
		const float WaitTime = FMath::FRandRange(PatrolWaitMin, PatrolWaitMax);
		GetWorldTimerManager().SetTimer(PatrolTimer, this, &ThisClass::PatrolTimerFinished, WaitTime);
	}
}

void AEnemy::PatrolTimerFinished()
{
	MoveToTarget(PatrolTarget);
}

void AEnemy::StartAttackTimer()
{
	EnemyState = EEnemyState::EES_Attacking;
	const float AttackTime = FMath::RandRange(AttackMin, AttackMax);
	GetWorldTimerManager().SetTimer(AttackTimer, this, &ThisClass::Attack, AttackTime);
}

void AEnemy::ClearAttackTimer()
{
	GetWorldTimerManager().ClearTimer(AttackTimer);
}

bool AEnemy::InTargetRange(AActor* Target, double Radius)
{
	if (!Target) return false;
	
	const double DistanceToTarget = (Target->GetActorLocation() - GetActorLocation()).Size();

	return DistanceToTarget <= Radius;
}

void AEnemy::MoveToTarget(AActor* Target)
{
	if (!EnemyController || !Target) return;
	
	FAIMoveRequest MoveRequest;
	MoveRequest.SetGoalActor(Target);
	MoveRequest.SetAcceptanceRadius(50.f);
			
	EnemyController->MoveTo(MoveRequest);
}

AActor* AEnemy::ChoosePatrolTarget()
{
	TArray<AActor*> ValidTargets;
	for (AActor* Target : PatrolTargets)
	{
		if (Target != PatrolTarget)
		{
			ValidTargets.AddUnique(Target);
		}
	}
		
	if (ValidTargets.Num() > 0)
	{
		const int32 TargetSelection = FMath::RandRange(0, ValidTargets.Num() - 1);
		return PatrolTargets[TargetSelection];
	}

	return nullptr;
}

void AEnemy::ToggleHealthBar(const bool bShow) const
{
	if (HealthBarWidget)
	{
		HealthBarWidget->SetVisibility(bShow);
	}
}

void AEnemy::LoseInterest()
{
	CombatTarget = nullptr;
	ToggleHealthBar(false);
}

void AEnemy::StartPatrolling()
{
	EnemyState = EEnemyState::EES_Patrolling;
	GetCharacterMovement()->MaxWalkSpeed = PatrollingSpeed;
	MoveToTarget(PatrolTarget);
}

void AEnemy::ChaseTarget()
{
	EnemyState = EEnemyState::EES_Chasing;
	GetCharacterMovement()->MaxWalkSpeed = ChasingSpeed;
	MoveToTarget(CombatTarget);
}

void AEnemy::ClearPatrolTimer()
{
	GetWorldTimerManager().ClearTimer(PatrolTimer);
}

void AEnemy::SpawnDefaultWeapon()
{
	UWorld* World = GetWorld();
	if (World && WeaponClass)
	{
		AWeapon* DefaultWeapon = World->SpawnActor<AWeapon>(WeaponClass);
		DefaultWeapon->Equip(GetMesh(), FName("RightHandSocket"), this, this);
		EquippedWeapon = DefaultWeapon;
	}
}

bool AEnemy::IsOutsideCombatRadius()
{
	return !InTargetRange(CombatTarget, CombatRadius);
}

bool AEnemy::IsOutsideAttackRadius()
{
	return !InTargetRange(CombatTarget, AttackRadius);
}

bool AEnemy::IsInsideAttackRadius()
{
	return InTargetRange(CombatTarget, AttackRadius);
}

bool AEnemy::IsChasing()
{
	return EnemyState == EEnemyState::EES_Chasing;
}

bool AEnemy::IsAttacking()
{
	return EnemyState == EEnemyState::EES_Attacking;
}

bool AEnemy::IsEngaged()
{
	return EnemyState == EEnemyState::EES_Engaged;
}

bool AEnemy::IsDead()
{
	return EnemyState == EEnemyState::EES_Dead;
}

void AEnemy::PawnSeen(APawn* SeenPawn)
{
	const bool bShouldChaseTarget = 
		EnemyState != EEnemyState::EES_Dead &&
		EnemyState != EEnemyState::EES_Chasing &&
		EnemyState < EEnemyState::EES_Attacking &&
		SeenPawn && SeenPawn->ActorHasTag(FName("EngageableTarget"));

	
	if (bShouldChaseTarget)
	{
		CombatTarget = SeenPawn;
		ClearPatrolTimer();
		ChaseTarget();
	}
}
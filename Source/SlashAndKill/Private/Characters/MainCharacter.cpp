// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/MainCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Components/InputComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/AttributeComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "HUD/MainHUD.h"
#include "HUD/SlashOverlay.h"
#include "Items/Item.h"
#include "Items/Soul.h"
#include "Items/Treasure.h"
#include "Items/Weapons/Weapon.h"

AMainCharacter::AMainCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);

	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
	GetMesh()->SetGenerateOverlapEvents(true);

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 300.f;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom);
}

void AMainCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Attributes && SlashOverlay)
	{
		Attributes->RegenStamina(DeltaTime);
		SlashOverlay->SetStaminaBarPercent(Attributes->GetStaminaPercent());
	}
}

void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (const APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(MainCharacterContext, 0);
		}

		InitializeSlashOverlay(PlayerController);
	}

	Tags.Add(FName("EngageableTarget"));
}

void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	// CastChecked will crash the game if the Cast failed
	if(UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ThisClass::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ThisClass::Look);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ThisClass::Jump);
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Triggered, this, &ThisClass::InteractPressed);
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &AMainCharacter::Attack);
		EnhancedInputComponent->BindAction(DodgeAction, ETriggerEvent::Triggered, this, &AMainCharacter::Dodge);
	}
}

void AMainCharacter::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	Super::GetHit_Implementation(ImpactPoint, Hitter);

	SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);
	if (Attributes && Attributes->GetHealthPercent() > 0.f)
		ActionState = EActionState::EAS_HitReaction;
}

float AMainCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
                                 AActor* DamageCauser)
{
	HandleDamage(DamageAmount);
	SetHUDHealth();
	
	return DamageAmount;
}

void AMainCharacter::Jump()
{
	if (ActionState != EActionState::EAS_Unoccupied) return;
	
	Super::Jump();
}

void AMainCharacter::SetOverlappingItem(AItem* Item)
{
	OverlappingItem = Item;
}

void AMainCharacter::AddSouls(ASoul* Soul)
{
	if (Attributes && SlashOverlay)
	{
		Attributes->AddSouls(Soul->GetSoulsAmount());
		SlashOverlay->SetSouls(Attributes->GetSouls());
	}
}

void AMainCharacter::AddGold(ATreasure* Treasure)
{
	if (Attributes && SlashOverlay)
	{
		Attributes->AddGold(Treasure->GetGoldAmount());
		SlashOverlay->SetGold(Attributes->GetGold());
	}
}

void AMainCharacter::InteractPressed()
{
	AWeapon* OverlappingWeapon = Cast<AWeapon>(OverlappingItem);
	
	if (OverlappingWeapon)
	{
		if (EquippedWeapon)
		{
			EquippedWeapon->Destroy();
			
		}
		EquipWeapon(OverlappingWeapon);
	}
	else
	{
		if (CanDisarm()) Disarm();
		else if (CanArm()) Arm();
	}
}

bool AMainCharacter::CanDisarm() const
{
	return ActionState == EActionState::EAS_Unoccupied && CharacterState != ECharacterState::ECS_Unequipped;
}

bool AMainCharacter::CanArm() const
{
	return ActionState == EActionState::EAS_Unoccupied && CharacterState == ECharacterState::ECS_Unequipped && EquippedWeapon;
}
 
void AMainCharacter::Disarm()
{
	PlayEquipMontage(FName("Unequip"));
	CharacterState = ECharacterState::ECS_Unequipped;
	ActionState = EActionState::EAS_EquippingWeapon;
}

void AMainCharacter::Arm()
{
	PlayEquipMontage(FName("Equip"));
	CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
	ActionState = EActionState::EAS_EquippingWeapon;
}

void AMainCharacter::AttachWeaponToBack()
{
	if (EquippedWeapon == nullptr) return;

	EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("SpineSocket"));
}

void AMainCharacter::AttachWeaponToHand()
{
	if (EquippedWeapon == nullptr) return;

	EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("RightHandSocket"));
}

void AMainCharacter::FinishEquipping()
{
	ActionState = EActionState::EAS_Unoccupied;
}

void AMainCharacter::HitReactEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
}

void AMainCharacter::Attack()
{
	Super::Attack();
	
	if (!CanAttack()) return;
	
	PlayAttackMontage();
	ActionState = EActionState::EAS_Attacking;
}

void AMainCharacter::Dodge()
{
	if (ActionState != EActionState::EAS_Unoccupied || !HasEnoughStamina()) return;

	Attributes->UseStamina(Attributes->GetDodgeCost());
	if (SlashOverlay)
		SlashOverlay->SetStaminaBarPercent(Attributes->GetStaminaPercent());
	
	ActionState = EActionState::EAS_Dodge;
	PlayDodgeMontage();
}

bool AMainCharacter::CanAttack()
{
	return ActionState == EActionState::EAS_Unoccupied && CharacterState != ECharacterState::ECS_Unequipped;
}

void AMainCharacter::PlayEquipMontage(const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	
	if (AnimInstance == nullptr || EquipMontage == nullptr) return;
	
	AnimInstance->Montage_Play(EquipMontage);
	AnimInstance->Montage_JumpToSection(SectionName, EquipMontage);
}

void AMainCharacter::Die()
{
	Super::Die();
	DisableCapsule();
	ActionState = EActionState::EAS_Dead;
}

bool AMainCharacter::HasEnoughStamina()
{
	return Attributes && Attributes->GetStamina() > Attributes->GetDodgeCost();
}

void AMainCharacter::EquipWeapon(AWeapon* Weapon)
{	
	Weapon->Equip(GetMesh(), FName("RightHandSocket"), this, this);
	CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
	EquippedWeapon = Weapon;
	OverlappingItem = nullptr;
}

void AMainCharacter::AttackEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
}

void AMainCharacter::DodgeEnd()
{
	Super::DodgeEnd();

	ActionState = EActionState::EAS_Unoccupied;
}

void AMainCharacter::Move(const FInputActionValue& Value)
{
	if (ActionState != EActionState::EAS_Unoccupied) return;
	
	const FVector2D DirectionVector = Value.Get<FVector2D>();

	if (GetController())
	{
		const FRotator Rotation = GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(ForwardDirection, DirectionVector.Y);
		AddMovementInput(RightDirection, DirectionVector.X);

		// Basic Way
		// const FVector Forward = GetActorForwardVector();
		// const FVector Right = GetActorRightVector();
		//
		// AddMovementInput(Forward, DirectionVector.Y);
		// AddMovementInput(Right, DirectionVector.X);
	}
}

void AMainCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxisValue = Value.Get<FVector2D>();
	
	AddControllerYawInput(LookAxisValue.X);
	AddControllerPitchInput(LookAxisValue.Y);
}

void AMainCharacter::InitializeSlashOverlay(const APlayerController* PlayerController)
{
	if (PlayerController == nullptr) return;
	
	AMainHUD* MainHUD = Cast<AMainHUD>(PlayerController->GetHUD());
	if (MainHUD)
	{
		SlashOverlay = MainHUD->GetSlashOverlay();

		if (SlashOverlay && Attributes)
		{
			SlashOverlay->SetHealthBarPercent(Attributes->GetHealthPercent());
			SlashOverlay->SetStaminaBarPercent(1.f);
			SlashOverlay->SetGold(0);
			SlashOverlay->SetSouls(0);
		}
	}
}

void AMainCharacter::SetHUDHealth()
{
	if (SlashOverlay && Attributes)
	{
		SlashOverlay->SetHealthBarPercent(Attributes->GetHealthPercent());
	}
}
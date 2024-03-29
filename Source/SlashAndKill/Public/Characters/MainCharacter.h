// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "InputActionValue.h"
#include "Characters/CharacterTypes.h"
#include "Interfaces/PickupInterface.h"
#include "MainCharacter.generated.h"

class USlashOverlay;
class AItem;
class ASoul;
class ATreasure;
class UInputMappingContext;
class UInputComponent;
class UInputAction;
class USpringArmComponent;
class UCameraComponent;
class UAnimMontage;

UCLASS()
class SLASHANDKILL_API AMainCharacter : public ABaseCharacter, public IPickupInterface
{
	GENERATED_BODY()

public:
	AMainCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	virtual void Jump() override;
	virtual void SetOverlappingItem(AItem* Item) override;
	virtual void AddSouls(ASoul* Soul) override;
	virtual void AddGold(ATreasure* Treasure) override;

protected:
	virtual void BeginPlay() override;
	
	/**	Callbacks for input */
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void InteractPressed();
	virtual void Attack() override;
	void Dodge();
	
	/**	Combat	*/
	void EquipWeapon(AWeapon* Weapon);
	virtual void AttackEnd() override;
	virtual void DodgeEnd() override;
	virtual bool CanAttack() override;
	bool CanDisarm() const;
	bool CanArm() const;
	void Disarm();
	void Arm();
	void PlayEquipMontage(const FName& SectionName);
	virtual void Die() override;
	bool HasEnoughStamina();
	
	UFUNCTION(BlueprintCallable)
	void AttachWeaponToBack();

	UFUNCTION(BlueprintCallable)
	void AttachWeaponToHand();

	UFUNCTION(BlueprintCallable)
	void FinishEquipping();

	UFUNCTION(BlueprintCallable)
	void HitReactEnd();

	/** Input */
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputMappingContext> MainCharacterContext;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> JumpAction;
	
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> InteractAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> AttackAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> DodgeAction;

private:
	void InitializeSlashOverlay(const APlayerController* PlayerController);
	void SetHUDHealth();
	
	/** Character Components*/
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCameraComponent> FollowCamera;

	UPROPERTY(VisibleInstanceOnly)
	TObjectPtr<AItem> OverlappingItem;
	
	/**	Animation Montages */
	
	UPROPERTY(EditDefaultsOnly, Category = Montages)
	TObjectPtr<UAnimMontage> EquipMontage;

	ECharacterState CharacterState = ECharacterState::ECS_Unequipped;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	EActionState ActionState = EActionState::EAS_Unoccupied;

	UPROPERTY()
	TObjectPtr<USlashOverlay> SlashOverlay;

public:
	FORCEINLINE ECharacterState GetCharacterState() const { return CharacterState; }
	FORCEINLINE EActionState GetActionState() const { return ActionState; }
	
};

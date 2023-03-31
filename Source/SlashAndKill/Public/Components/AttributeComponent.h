// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttributeComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SLASHANDKILL_API UAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UAttributeComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void RegenStamina(float DeltaTime);
	
protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, Category = Attributes)
	float Health { 100.f };

	UPROPERTY(EditAnywhere, Category = Attributes)
	float MaxHealth { 100.f };

	UPROPERTY(EditAnywhere, Category = Attributes)
	int32 Gold { 0 };

	UPROPERTY(EditAnywhere, Category = Attributes)
	int32 Souls { 0 };

	UPROPERTY(EditAnywhere, Category = Attributes)
	float Stamina { 100.f };

	UPROPERTY(EditAnywhere, Category = Attributes)
	float MaxStamina { 100.f };

	UPROPERTY(EditAnywhere, Category = Attributes)
	float DodgeCost { 15.f };

	UPROPERTY(EditAnywhere, Category = Attributes)
	float StaminaRegenRate { 8.f };

public:
	void ReceiveDamage(float Damage);
	float GetHealthPercent() const;
	void UseStamina(float StaminaCost);
	float GetStaminaPercent() const;
	bool IsAlive() const;
	void AddSouls(int32 SoulsAmount);
	void AddGold(int32 GoldAmount);
	FORCEINLINE int32 GetGold() const { return Gold; }
	FORCEINLINE int32 GetSouls() const { return Souls; }
	FORCEINLINE float GetStamina() const { return Stamina; }
	FORCEINLINE float GetDodgeCost() const { return DodgeCost; }
};

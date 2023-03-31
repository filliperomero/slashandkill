// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/AttributeComponent.h"

UAttributeComponent::UAttributeComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UAttributeComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UAttributeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UAttributeComponent::RegenStamina(float DeltaTime)
{
	if (GetStamina() < MaxStamina)
		Stamina = FMath::Clamp(Stamina + StaminaRegenRate * DeltaTime, 0.f, MaxStamina);
}

void UAttributeComponent::ReceiveDamage(float Damage)
{
	Health = FMath::Clamp(Health - Damage, 0.f, MaxHealth);
}

float UAttributeComponent::GetHealthPercent() const
{	
	return Health / MaxHealth;
}

void UAttributeComponent::UseStamina(float StaminaCost)
{
	Stamina = FMath::Clamp(Stamina - StaminaCost, 0.f, MaxStamina);
}

float UAttributeComponent::GetStaminaPercent() const
{
	return Stamina / MaxStamina;
}

bool UAttributeComponent::IsAlive() const
{
	return Health > 0.f;
}

void UAttributeComponent::AddSouls(int32 SoulsAmount)
{
	Souls += SoulsAmount;
}

void UAttributeComponent::AddGold(int32 GoldAmount)
{
	Gold += GoldAmount;
}

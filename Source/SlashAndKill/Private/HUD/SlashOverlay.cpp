// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/SlashOverlay.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void USlashOverlay::SetHealthBarPercent(float Percent)
{
	if (HealthBarProgressBar == nullptr) return;
	
	HealthBarProgressBar->SetPercent(Percent);
}

void USlashOverlay::SetStaminaBarPercent(float Percent)
{
	if (StaminaProgressBar == nullptr) return;
	
	StaminaProgressBar->SetPercent(Percent);
}

void USlashOverlay::SetGold(int32 GoldAmount)
{
	if (GoldText == nullptr) return;

	GoldText->SetText(FText::FromString(FString::Printf(TEXT("%d"), GoldAmount)));
}

void USlashOverlay::SetSouls(int32 SoulsAmount)
{
	if (SoulsText == nullptr) return;

	SoulsText->SetText(FText::FromString(FString::Printf(TEXT("%d"), SoulsAmount)));
}

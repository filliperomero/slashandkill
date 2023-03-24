// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SlashOverlay.generated.h"

class UTextBlock;
class UProgressBar;
/**
 * 
 */
UCLASS()
class SLASHANDKILL_API USlashOverlay : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetHealthBarPercent(float Percent);
	void SetStaminaBarPercent(float Percent);
	void SetGold(int32 GoldAmount);
	void SetSouls(int32 SoulsAmount);

protected:

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> HealthBarProgressBar;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> StaminaProgressBar;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> GoldText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> SoulsText;
	
};

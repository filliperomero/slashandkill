// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MainHUD.generated.h"

class USlashOverlay;
/**
 * 
 */
UCLASS()
class SLASHANDKILL_API AMainHUD : public AHUD
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	
private:
	UPROPERTY(EditDefaultsOnly, Category = Slash)
	TSubclassOf<USlashOverlay> SlashOverlayClass;

	UPROPERTY()
	TObjectPtr<USlashOverlay> SlashOverlay;

public:
	FORCEINLINE USlashOverlay* GetSlashOverlay() const { return SlashOverlay; }
};

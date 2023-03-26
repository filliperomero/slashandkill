// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/MainHUD.h"
#include "HUD/SlashOverlay.h"

#include "Blueprint/UserWidget.h"

void AMainHUD::BeginPlay()
{
	Super::BeginPlay();

	UWorld* World = GetWorld();

	if (World)
	{
		APlayerController* PlayerController = World->GetFirstPlayerController();

		if (PlayerController && SlashOverlayClass)
		{
			SlashOverlay = CreateWidget<USlashOverlay>(PlayerController, SlashOverlayClass);
			SlashOverlay->AddToViewport();
		}
	}
}

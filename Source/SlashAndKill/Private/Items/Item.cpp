// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Item.h"
#include "SlashAndKill/DebugMacros.h"

AItem::AItem()
{
	PrimaryActorTick.bCanEverTick = true;
	
	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMeshComponent"));
	RootComponent = ItemMesh;
}

void AItem::BeginPlay()
{
	Super::BeginPlay();
}

float AItem::TransformedSin()
{
	return FMath::Sin(RunningTime * TimeConstant) * Amplitude;
}

float AItem::TransformedCos()
{
	return FMath::Cos(RunningTime * TimeConstant) * Amplitude;
}

void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	RunningTime += DeltaTime;
}


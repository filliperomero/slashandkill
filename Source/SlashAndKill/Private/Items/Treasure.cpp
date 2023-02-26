// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Treasure.h"
#include "Characters/MainCharacter.h"
#include "Kismet/GameplayStatics.h"

void ATreasure::BeginPlay()
{
	Super::BeginPlay();

	ItemState = EItemState::EIS_Unequipped;
}

void ATreasure::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AMainCharacter* MainCharacter = Cast<AMainCharacter>(OtherActor);
	if (MainCharacter)
	{
		if (PickupSound)
			UGameplayStatics::PlaySoundAtLocation(this, PickupSound, GetActorLocation());
		
		Destroy();
	}
}

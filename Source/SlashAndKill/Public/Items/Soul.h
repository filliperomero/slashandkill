// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Soul.generated.h"

class UNiagaraSystem;
/**
 * 
 */
UCLASS()
class SLASHANDKILL_API ASoul : public AItem
{
	GENERATED_BODY()

public:
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;
	virtual void OnSphereOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	) override;

private:
	UPROPERTY(EditAnywhere, Category = "Souls Properties")
	int32 SoulsAmount { 1 };

	UPROPERTY(EditAnywhere, Category = "Souls Properties")
	float DriftRate { -15.f };

	double DesiredZ { 0.f };

public:
	FORCEINLINE int32 GetSoulsAmount() const { return SoulsAmount; }
	FORCEINLINE void SetSouls(int32 NumberOfSouls) { SoulsAmount = NumberOfSouls; }
};

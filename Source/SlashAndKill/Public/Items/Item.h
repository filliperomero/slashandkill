// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

UCLASS()
class SLASHANDKILL_API AItem : public AActor
{
	GENERATED_BODY()
	
public:
	AItem();
	virtual void Tick(float DeltaTime) override;
	
protected:
	virtual void BeginPlay() override;

private:
	UFUNCTION(BlueprintPure)
	float TransformedSin();

	UFUNCTION(BlueprintPure)
	float TransformedCos();

	template<typename T>
	T Avg(T First, T Second);

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* ItemMesh;
	
public:
	UPROPERTY(VisibleInstanceOnly)
	float RunningTime { 0.f };

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float Amplitude { 5.f };

	UPROPERTY(EditInstanceOnly)
	float TimeConstant { .25f };
};

template <typename T>
T AItem::Avg(T First, T Second)
{
	return (First + Second) / 2;
}

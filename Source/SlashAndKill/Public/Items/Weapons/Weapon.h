// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Weapon.generated.h"

class USoundBase;
class UBoxComponent;
class USceneComponent;

/**
 * 
 */
UCLASS()
class SLASHANDKILL_API AWeapon : public AItem
{
	GENERATED_BODY()

public:
	AWeapon();
	void Equip(USceneComponent* InParent, FName InSocketName, AActor* NewOwner, APawn* NewInstigator);
	void AttachMeshToSocket(USceneComponent* InParent, const FName& InSocketName);

	TArray<AActor*> IgnoreActors;

protected:
	virtual void BeginPlay() override;
	
	UFUNCTION()
	void OnWeaponBoxOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	UFUNCTION()
	void OnWeaponBoxEndOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
	);

	UFUNCTION(BlueprintImplementableEvent)
	void CreateFields(const FVector& FieldLocation);

private:
	void PlayEquipSound();
	void DisableAreaSphereCollision();
	void DeactivateEmbers();
	void WeaponBoxTrace(FHitResult& WeaponBoxHit);
	void ExecuteGetHit(FHitResult& WeaponBoxHit);

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	FVector WeaponBoxTraceExtent = FVector(5.f);

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	bool bShowWeaponBoxDebug = false;
	
	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	TObjectPtr<USoundBase> EquipSound;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	TObjectPtr<UBoxComponent> WeaponBox;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	TObjectPtr<USceneComponent> TraceStartComponent;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	TObjectPtr<USceneComponent> TraceEndComponent;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	float Damage { 20.f };

public:
	FORCEINLINE UBoxComponent* GetWeaponBox() const { return WeaponBox; }
	
};

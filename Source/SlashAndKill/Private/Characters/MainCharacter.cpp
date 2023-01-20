// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/MainCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Components/InputComponent.h"

AMainCharacter::AMainCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (const APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(MainCharacterContext, 0);
		}
	}
}

void AMainCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	// CastChecked will crash the game if the Cast failed
	if(UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ThisClass::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ThisClass::Look);
	}
}

void AMainCharacter::Move(const FInputActionValue& Value)
{
	const FVector2D DirectionVector = Value.Get<FVector2D>();

	if (GetController())
	{
		const FRotator Rotation = GetController()->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(ForwardDirection, DirectionVector.Y);
		AddMovementInput(RightDirection, DirectionVector.X);

		// Basic Way
		// const FVector Forward = GetActorForwardVector();
		// const FVector Right = GetActorRightVector();
		//
		// AddMovementInput(Forward, DirectionVector.Y);
		// AddMovementInput(Right, DirectionVector.X);
	}
}

void AMainCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxisValue = Value.Get<FVector2D>();

	if (GetController())
	{                                               
		AddControllerYawInput(LookAxisValue.X);
		AddControllerPitchInput(LookAxisValue.Y);
	}
}

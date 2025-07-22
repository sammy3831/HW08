// Fill out your copyright notice in the Description page of Project Settings.

#include "SpartaCharacter.h"
#include "EnhancedInputComponent.h"
#include "SpartaGameState.h"
#include "SpartaPlayerController.h"
#include "Camera/CameraComponent.h"
#include "Components/TextBlock.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

ASpartaCharacter::ASpartaCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->TargetArmLength = 300.0f;
	SpringArmComp->bUsePawnControlRotation = true;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
	CameraComp->bUsePawnControlRotation = false;

	OverheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverHeadWidget"));
	OverheadWidget->SetupAttachment(GetMesh());
	OverheadWidget->SetWidgetSpace(EWidgetSpace::Screen);

	DefaultSpeed = 600.0f;
	WalkSpeed = DefaultSpeed;
	SprintSpeedMultiplier = 1.7f;
	SprintSpeed = WalkSpeed * SprintSpeedMultiplier;

	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;

	MaxHealth = 100.0f;
	Health = MaxHealth;
}

float ASpartaCharacter::GetHealth() const
{
	return Health;
}

void ASpartaCharacter::AddHealth(float Amount)
{
	Health = FMath::Clamp(Health + Amount, 0.0f, MaxHealth);
	UpdateOverheadHP();
}

void ASpartaCharacter::ApplyChangeSpeedAffect(float Amount, float Duration, const FName& ItemType)
{
	if (GetCharacterMovement())
	{
		WalkSpeed = WalkSpeed * Amount;
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;

		if (ItemType == "SlowItem")
		{
			AffectCount.DecreaseSpeedCount++;
			GetWorld()->GetTimerManager().SetTimer(DecreaseSpeedTimerHandle, this,
			                                       &ASpartaCharacter::ResetSpeedFromSlow,
			                                       Duration, false);
		}
		else if (ItemType == "SpeedItem")
		{
			AffectCount.IncreaseSpeedCount++;
			GetWorld()->GetTimerManager().SetTimer(IncreaseSpeedTimerHandle, this,
			                                       &ASpartaCharacter::ResetSpeedFromSpeed,
			                                       Duration, false);
		}
	}
}

void ASpartaCharacter::ResetSpeedFromSpeed()
{
	if (GetCharacterMovement())
	{
		AffectCount.IncreaseSpeedCount = 0;
		WalkSpeed = DefaultSpeed;
		GetCharacterMovement()->MaxWalkSpeed = DefaultSpeed;
	}
}

void ASpartaCharacter::ResetSpeedFromSlow()
{
	if (GetCharacterMovement())
	{
		AffectCount.DecreaseSpeedCount = 0;
		WalkSpeed = DefaultSpeed;
		GetCharacterMovement()->MaxWalkSpeed = DefaultSpeed;
	}
}

void ASpartaCharacter::ApplyChangeScaleAffect(float Amount, float Duration)
{
	SetActorScale3D(GetActorScale3D() * Amount);

	AffectCount.ChangeScaleCount++;

	GetWorld()->GetTimerManager().SetTimer(ChangeScaleTimerHandle, this,
	                                       &ASpartaCharacter::ResetScale,
	                                       Duration, false);
}

void ASpartaCharacter::ResetScale()
{
	AffectCount.ChangeScaleCount = 0;
	SetActorScale3D(FVector(1.0f, 1.0f, 1.0f));
}

void ASpartaCharacter::BeginPlay()
{
	Super::BeginPlay();
	UpdateOverheadHP();
}

void ASpartaCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (ASpartaPlayerController* PlayerController = Cast<ASpartaPlayerController>(GetController()))
		{
			if (PlayerController->MoveAction)
			{
				EnhancedInput->BindAction(PlayerController->MoveAction, ETriggerEvent::Triggered,
				                          this, &ASpartaCharacter::Move);
			}
			if (PlayerController->JumpAction)
			{
				EnhancedInput->BindAction(PlayerController->JumpAction, ETriggerEvent::Triggered,
				                          this, &ASpartaCharacter::StartJump);
				EnhancedInput->BindAction(PlayerController->JumpAction, ETriggerEvent::Completed,
				                          this, &ASpartaCharacter::StopJump);
			}
			if (PlayerController->LookAction)
			{
				EnhancedInput->BindAction(PlayerController->LookAction, ETriggerEvent::Triggered,
				                          this, &ASpartaCharacter::Look);
			}
			if (PlayerController->SprintAction)
			{
				EnhancedInput->BindAction(PlayerController->SprintAction, ETriggerEvent::Triggered,
				                          this, &ASpartaCharacter::StartSprint);
				EnhancedInput->BindAction(PlayerController->SprintAction, ETriggerEvent::Completed,
				                          this, &ASpartaCharacter::StopSprint);
			}
		}
	}
}

float ASpartaCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
                                   AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	Health = FMath::Clamp(Health - ActualDamage, 0.0f, MaxHealth);
	UpdateOverheadHP();

	if (Health <= 0.0f)
	{
		OnDeath();
	}

	return ActualDamage;
}

void ASpartaCharacter::Move(const FInputActionValue& Value)
{
	if (!Controller) return;

	const FVector2D MoveInput = Value.Get<FVector2D>();

	if (!FMath::IsNearlyZero(MoveInput.X))
	{
		AddMovementInput(GetActorForwardVector(), MoveInput.X);
	}
	if (!FMath::IsNearlyZero(MoveInput.Y))
	{
		AddMovementInput(GetActorRightVector(), MoveInput.Y);
	}
}

void ASpartaCharacter::StartJump(const FInputActionValue& Value)
{
	if (Value.Get<bool>())
	{
		Jump();
	}
}

void ASpartaCharacter::StopJump(const FInputActionValue& Value)
{
	if (!Value.Get<bool>())
	{
		StopJumping();
	}
}

void ASpartaCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookInput = Value.Get<FVector2D>();

	AddControllerYawInput(LookInput.X);
	AddControllerPitchInput(LookInput.Y);
}

void ASpartaCharacter::StartSprint(const FInputActionValue& Value)
{
	if (GetCharacterMovement())
	{
		SprintSpeed = WalkSpeed * SprintSpeedMultiplier;
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
	}
}

void ASpartaCharacter::StopSprint(const FInputActionValue& Value)
{
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	}
}

void ASpartaCharacter::OnDeath()
{
	ASpartaGameState* SpartaGameState = GetWorld() ? GetWorld()->GetGameState<ASpartaGameState>() : nullptr;
	if (SpartaGameState)
	{
		SpartaGameState->OnGameOver();
	}
}

void ASpartaCharacter::UpdateOverheadHP()
{
	if (!OverheadWidget) return;

	UUserWidget* OverheadWidgetInstance = OverheadWidget->GetUserWidgetObject();

	if (!OverheadWidgetInstance) return;

	if (UTextBlock* HPText = Cast<UTextBlock>(OverheadWidgetInstance->GetWidgetFromName(TEXT("OverHeadHP"))))
	{
		HPText->SetText(FText::FromString(FString::Printf(TEXT("%.0f / %.0f"), Health, MaxHealth)));
	}
}

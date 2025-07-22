// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SpartaCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UWidgetComponent;
struct FInputActionValue;

UCLASS()
class SPARTAPROJECT_API ASpartaCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ASpartaCharacter();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	USpringArmComponent* SpringArmComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* CameraComp;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	UWidgetComponent* OverheadWidget;

	UFUNCTION(BlueprintPure, Category = "Health")
	float GetHealth() const;
	
	UFUNCTION(BlueprintCallable, Category = "Health")
	void AddHealth(float Amount);

	void ApplyChangeSpeedAffect(float Amount, float Duration, const FName& ItemType);
	void ApplyChangeScaleAffect(float Amount, float Duration);

	struct FAffectCount
	{
		int32 IncreaseSpeedCount = 0;
		int32 DecreaseSpeedCount = 0;
		int32 ChangeScaleCount = 0;
	};

	FAffectCount AffectCount;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float MaxHealth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float Health;

	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual float TakeDamage(float DamageAmount,
	                         struct FDamageEvent const& DamageEvent,
	                         AController* EventInstigator,
	                         AActor* DamageCauser) override;

	UFUNCTION()
	void Move(const FInputActionValue& Value);
	UFUNCTION()
	void StartJump(const FInputActionValue& Value);
	UFUNCTION()
	void StopJump(const FInputActionValue& Value);
	UFUNCTION()
	void Look(const FInputActionValue& Value);
	UFUNCTION()
	void StartSprint(const FInputActionValue& Value);
	UFUNCTION()
	void StopSprint(const FInputActionValue& Value);

	void OnDeath();
	void UpdateOverheadHP();

private:
	float DefaultSpeed;
	float WalkSpeed;
	float SprintSpeedMultiplier;
	float SprintSpeed;

	FTimerHandle IncreaseSpeedTimerHandle;
	FTimerHandle DecreaseSpeedTimerHandle;
	FTimerHandle ChangeScaleTimerHandle;
	
	void ResetSpeedFromSpeed();
	void ResetSpeedFromSlow();
	void ResetScale();
};

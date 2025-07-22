// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ChangeSpeedItem.h"
#include "SpeedItem.generated.h"

UCLASS()
class SPARTAPROJECT_API ASpeedItem : public AChangeSpeedItem
{
	GENERATED_BODY()

public:
	ASpeedItem();

	virtual void ActivateItem(AActor* Activator) override;
};

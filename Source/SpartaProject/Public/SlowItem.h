// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ChangeSpeedItem.h"
#include "SlowItem.generated.h"

UCLASS()
class SPARTAPROJECT_API ASlowItem : public AChangeSpeedItem
{
	GENERATED_BODY()

public:
	ASlowItem();

	virtual void ActivateItem(AActor* Activator) override;
	
};

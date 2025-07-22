// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "ChangeSpeedItem.generated.h"

UCLASS()
class SPARTAPROJECT_API AChangeSpeedItem : public ABaseItem
{
	GENERATED_BODY()

public:
	AChangeSpeedItem();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	float ChangeSpeedAmount;

	virtual void ActivateItem(AActor* Activator) override;
	
	
};

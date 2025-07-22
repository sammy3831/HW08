
#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "DecreaseScaleItem.generated.h"

UCLASS()
class SPARTAPROJECT_API ADecreaseScaleItem : public ABaseItem
{
	GENERATED_BODY()

public:
	ADecreaseScaleItem();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	float DecreaseScaleAmount;

	virtual void ActivateItem(AActor* Activator) override;
};

#pragma once

#include "CoreMinimal.h"
#include "MineItem.h"
#include "RandomExplosionItem.generated.h"

UCLASS()
class SPARTAPROJECT_API ARandomExplosionItem : public AMineItem
{
    GENERATED_BODY()

public:
    ARandomExplosionItem();

    float ExplosionTimerDelay;

protected:
    virtual void BeginPlay() override;
    virtual void ActivateItem(AActor* Activator) override;

private:
    FTimerHandle DelayExplosionTimerHandle; 
};
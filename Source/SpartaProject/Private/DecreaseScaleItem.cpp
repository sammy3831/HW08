
#include "DecreaseScaleItem.h"

#include "SpartaCharacter.h"

ADecreaseScaleItem::ADecreaseScaleItem()
{
	DecreaseScaleAmount = 0.5f;
	ItemType = "DecreaseScaleItem";
}

void ADecreaseScaleItem::ActivateItem(AActor* Activator)
{
	Super::ActivateItem(Activator);

	if (ASpartaCharacter* playerCharacter = Cast<ASpartaCharacter>(Activator))
	{
		playerCharacter->ApplyChangeScaleAffect(DecreaseScaleAmount, 5.0f);
	}
	DestroyItem();
}

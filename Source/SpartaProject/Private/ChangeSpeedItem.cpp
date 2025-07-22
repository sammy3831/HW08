
#include "ChangeSpeedItem.h"

#include "SpartaCharacter.h"

AChangeSpeedItem::AChangeSpeedItem()
{
	ChangeSpeedAmount = 0.0f;
	ItemType = "ChangeSpeedItem";
}

void AChangeSpeedItem::ActivateItem(AActor* Activator)
{
	Super::ActivateItem(Activator);

	if (ASpartaCharacter* playerCharacter = Cast<ASpartaCharacter>(Activator))
	{
		playerCharacter->ApplyChangeSpeedAffect(ChangeSpeedAmount, 5.0f, ItemType);
	}
	DestroyItem();
}

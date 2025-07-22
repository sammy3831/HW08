
#include "SpeedItem.h"
#include "SpartaCharacter.h"

ASpeedItem::ASpeedItem()
{
	ChangeSpeedAmount = 1.3f;
	ItemType = "SpeedItem";
}

void ASpeedItem::ActivateItem(AActor* Activator)
{
	Super::ActivateItem(Activator);
}

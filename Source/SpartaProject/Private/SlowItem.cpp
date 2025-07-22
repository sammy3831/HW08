#include "SlowItem.h"
#include "SpartaCharacter.h"

ASlowItem::ASlowItem()
{
	ChangeSpeedAmount = 0.7f;
	ItemType = "SlowItem";
}

void ASlowItem::ActivateItem(AActor* Activator)
{
	Super::ActivateItem(Activator);
}

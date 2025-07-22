
#include "RandomExplosionItem.h"

ARandomExplosionItem::ARandomExplosionItem()
{
	ExplosionTimerDelay = FMath::FRandRange(5.0f, 25.0f);
}

void ARandomExplosionItem::BeginPlay()
{
	Super::BeginPlay();

	GetWorld()->GetTimerManager().SetTimer(DelayExplosionTimerHandle, this,
		&AMineItem::Explode, ExplosionTimerDelay, false);
}

void ARandomExplosionItem::ActivateItem(AActor* Activator)
{
	Super::ActivateItem(Activator);
}

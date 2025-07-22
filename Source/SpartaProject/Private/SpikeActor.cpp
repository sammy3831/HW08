#include "SpikeActor.h"
#include "SpartaGameState.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"

ASpikeActor::ASpikeActor()
{
	PrimaryActorTick.bCanEverTick = false;

	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	SetRootComponent(Scene);

	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	Collision->SetCollisionProfileName("OverlapAllDynamic");
	Collision->SetupAttachment(Scene);

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetupAttachment(Collision);

	Collision->OnComponentBeginOverlap.AddDynamic(this, &ASpikeActor::OnActorOverlap);
	Collision->OnComponentEndOverlap.AddDynamic(this, &ASpikeActor::OnActorEndOverlap);

	SpikeStartLocation = FVector::ZeroVector;
	SpikeEndLocation = FVector::ZeroVector;
	GameState = nullptr;
}

void ASpikeActor::BeginPlay()
{
	Super::BeginPlay();

	GameState = GetWorld()->GetGameState<ASpartaGameState>();

	SpikeStartLocation = GetActorLocation();
	SpikeEndLocation = SpikeStartLocation + FVector(0.0f, 0.0f, 100.0f);

	GetWorld()->GetTimerManager().SetTimer(ActivateSpikeHandle, this,
	                                       &ASpikeActor::ReSetSpikeLocation,
	                                       5.0f, false);
}

void ASpikeActor::OnActorOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
                                bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor->ActorHasTag("Player"))
	{
		UGameplayStatics::ApplyDamage(OtherActor, 10, nullptr,
		                              this, UDamageType::StaticClass());
	}
}

void ASpikeActor::OnActorEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                   UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}

void ASpikeActor::SetSpikeLocation()
{
	SetActorLocation(SpikeEndLocation);
	GameState->bSpikeActivate = true;

	GetWorld()->GetTimerManager().SetTimer(ReSetSpikeHandle, this,
	                                       &ASpikeActor::ReSetSpikeLocation,
	                                       5.0f, false);
}

void ASpikeActor::ReSetSpikeLocation()
{
	SetActorLocation(SpikeStartLocation);
	GameState->bSpikeActivate = false;

	GetWorld()->GetTimerManager().SetTimer(ActivateSpikeHandle, this,
	                                       &ASpikeActor::SetSpikeLocation,
	                                       5.0f, false);
}
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpikeActor.generated.h"

class ASpartaGameState;
class USphereComponent;

UCLASS()
class SPARTAPROJECT_API ASpikeActor : public AActor
{
	GENERATED_BODY()

public:
	ASpikeActor();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item|Component")
	USceneComponent* Scene;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item|Component")
	UStaticMeshComponent* StaticMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item|Component")
	USphereComponent* Collision;

	void SetSpikeLocation();
	void ReSetSpikeLocation();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnActorOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	                    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	                    bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnActorEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	                       UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:
	FVector SpikeStartLocation;
	FVector SpikeEndLocation;

	FTimerHandle ActivateSpikeHandle;
	FTimerHandle ReSetSpikeHandle;

	UPROPERTY()
	ASpartaGameState* GameState;
};

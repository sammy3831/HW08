// Fill out your copyright notice in the Description page of Project Settings.

#include "SpartaGameState.h"
#include "CoinItem.h"
#include "SpartaCharacter.h"
#include "SpartaGameInstance.h"
#include "SpartaPlayerController.h"
#include "SpawnVolume.h"
#include "Kismet/GameplayStatics.h"
#include "Components/TextBlock.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"

ASpartaGameState::ASpartaGameState()
{
	Score = 0;
	SpawnedCoinCount = 0;
	CollectedCoinCount = 0;
	WaveDuration = 30.0f;
	CurrentLevelIndex = 0;
	MaxLevels = 3;
	MaxWaves = 3;
	CurrentWave = 0;
	bIsWaving = false;
}

void ASpartaGameState::BeginPlay()
{
	Super::BeginPlay();


	StartLevel();

	FString CurrentMapName = GetWorld()->GetMapName();
	if (!CurrentMapName.Contains("MenuLevel"))
	{
		StartWaveAfterDelay();
	}

	GetWorldTimerManager().SetTimer(HUDUpdateTimerHandle, this,
	                                &ASpartaGameState::UpdateHUD, 0.1f, true);
}

int32 ASpartaGameState::GetScore() const
{
	return Score;
}

void ASpartaGameState::AddScore(int32 Amount)
{
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance);
		if (SpartaGameInstance)
		{
			SpartaGameInstance->AddToScore(Amount);
		}
	}
}

void ASpartaGameState::OnGameOver()
{
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (ASpartaPlayerController* SpartaPlayerController = Cast<ASpartaPlayerController>(PlayerController))
		{
			SpartaPlayerController->SetPause(true);
			SpartaPlayerController->ShowMainMenu(true);
		}
	}
}

void ASpartaGameState::StartLevel()
{
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (ASpartaPlayerController* SpartaPlayerController = Cast<ASpartaPlayerController>(PlayerController))
		{
			SpartaPlayerController->ShowGameHUD();
		}
	}

	if (UGameInstance* GameInstance = GetGameInstance())
	{
		USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance);
		if (SpartaGameInstance)
		{
			CurrentLevelIndex = SpartaGameInstance->CurrentLevelIndex;
		}
	}
}

void ASpartaGameState::OnWaveTimeUp()
{
	GetWorldTimerManager().ClearTimer(WaveTimerHandle);
	EndWave();
}

void ASpartaGameState::OnCoinCollected()
{
	CollectedCoinCount++;
	UE_LOG(LogTemp, Warning, TEXT("Coin Collected: %d / %d"),
	       CollectedCoinCount, SpawnedCoinCount);
	if (SpawnedCoinCount > 0 && CollectedCoinCount >= SpawnedCoinCount)
	{
		GetWorldTimerManager().ClearTimer(WaveTimerHandle);
		EndWave();
	}
}

void ASpartaGameState::EndLevel()
{
	GetWorldTimerManager().ClearTimer(WaveTimerHandle);

	if (UGameInstance* GameInstance = GetGameInstance())
	{
		USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance);
		if (SpartaGameInstance)
		{
			AddScore(Score);
			CurrentLevelIndex++;
			SpartaGameInstance->CurrentLevelIndex = CurrentLevelIndex;
		}
	}

	if (CurrentLevelIndex >= MaxLevels)
	{
		OnGameOver();
		return;
	}

	if (LevelMapNames.IsValidIndex(CurrentLevelIndex))
	{
		UGameplayStatics::OpenLevel(GetWorld(), LevelMapNames[CurrentLevelIndex]);
	}
	else
	{
		OnGameOver();
	}
}

void ASpartaGameState::UpdateHUD()
{
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (ASpartaPlayerController* SpartaPlayerController = Cast<ASpartaPlayerController>(PlayerController))
		{
			if (UUserWidget* HUDWidget = SpartaPlayerController->GetHUDWidget())
			{
				if (UTextBlock* TimeText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Time"))))
				{
					float RemainingTime = GetWorldTimerManager().GetTimerRemaining(WaveTimerHandle);
					if (RemainingTime < 0.0f)
					{
						RemainingTime = 0.0f;
					}

					TimeText->SetText(FText::FromString(FString::Printf(TEXT("Time: %.1f"), RemainingTime)));
				}

				if (UTextBlock* ScoreText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Score"))))
				{
					if (UGameInstance* GameInstance = GetGameInstance())
					{
						USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance);
						if (SpartaGameInstance)
						{
							ScoreText->SetText(
								FText::FromString(FString::Printf(TEXT("Score: %d"), SpartaGameInstance->TotalScore)));
						}
					}
				}

				if (UTextBlock* LevelIndexText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Level"))))
				{
					LevelIndexText->SetText(
						FText::FromString(FString::Printf(TEXT("Level: %d"), CurrentLevelIndex + 1)));
				}

				if (UTextBlock* WaveText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Wave"))))
				{
					if (bIsWaving)
					{
						WaveText->SetText(FText::FromString(FString::Printf(TEXT("Wave: %d"), CurrentWave + 1)));
					}
					else
					{
						float RemainingTime = GetWorldTimerManager().GetTimerRemaining(WaveDelayTimerHandle);
						WaveText->SetText(
							FText::FromString(FString::Printf(TEXT("%.1f초 후 다음 웨이브를 시작합니다!"), RemainingTime)));
					}
				}

				if (UProgressBar* RemainingTimeBar = Cast<UProgressBar>(
					HUDWidget->GetWidgetFromName(TEXT("RemainingTimeBar"))))
				{
					float RemainingTime = GetWorldTimerManager().GetTimerRemaining(WaveTimerHandle);
					if (RemainingTime < 0.0f)
					{
						RemainingTime = 0.0f;
					}

					float RemainingTimePercent = RemainingTime / WaveDuration;
					RemainingTimeBar->SetPercent(RemainingTimePercent);
				}

				if (UTextBlock* IncreaseSpeedText = Cast<UTextBlock>(
					HUDWidget->GetWidgetFromName(TEXT("IncreaseSpeedText"))))
				{
					if (ASpartaCharacter* Character = Cast<ASpartaCharacter>(SpartaPlayerController->GetCharacter()))
					{
						IncreaseSpeedText->SetText(FText::FromString(FString::Printf(TEXT("속도 증가 : %d 중첩"), Character->AffectCount.IncreaseSpeedCount)));
					}
				}

				if (UTextBlock* DncreaseSpeedText = Cast<UTextBlock>(
					HUDWidget->GetWidgetFromName(TEXT("DecreaseSpeedText"))))
				{
					if (ASpartaCharacter* Character = Cast<ASpartaCharacter>(SpartaPlayerController->GetCharacter()))
					{
						DncreaseSpeedText->SetText(FText::FromString(FString::Printf(TEXT("속도 감소 : %d 중첩"), Character->AffectCount.DecreaseSpeedCount)));
					}
				}

				if (UTextBlock* ChangeScaleText = Cast<UTextBlock>(
					HUDWidget->GetWidgetFromName(TEXT("ChangeScaleText"))))
				{
					if (ASpartaCharacter* Character = Cast<ASpartaCharacter>(SpartaPlayerController->GetCharacter()))
					{
						ChangeScaleText->SetText(FText::FromString(FString::Printf(TEXT("크기 감소 : %d 중첩"), Character->AffectCount.ChangeScaleCount)));
					}
				}
			}
		}
	}
}

void ASpartaGameState::StartWave()
{
	SpawnedCoinCount = 0;
	CollectedCoinCount = 0;
	bIsWaving = true;

	TArray<AActor*> FoundVolumes;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnVolume::StaticClass(), FoundVolumes);

	for (int32 i = 0; i < SpawnCountEachWave[CurrentWave]; i++)
	{
		if (FoundVolumes.Num() > 0)
		{
			ASpawnVolume* SpawnVolume = Cast<ASpawnVolume>(FoundVolumes[0]);
			if (SpawnVolume)
			{
				AActor* SpawnActor = SpawnVolume->SpawnRandomItem();
				if (SpawnActor && SpawnActor->IsA(ACoinItem::StaticClass()))
				{
					SpawnedCoinCount++;
				}
			}
		}
	}

	GetWorldTimerManager().SetTimer(WaveTimerHandle, this,
	                                &ASpartaGameState::OnWaveTimeUp, WaveDuration, false);

	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red,
	                                 FString::Printf(TEXT("Wave %d 시작!"), CurrentWave + 1));
}

void ASpartaGameState::EndWave()
{
	bIsWaving = false;

	TArray<AActor*> FoundItems;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABaseItem::StaticClass(), FoundItems);

	for (int i = 0; i < FoundItems.Num(); i++)
	{
		ABaseItem* FoundItem = Cast<ABaseItem>(FoundItems[i]);
		if (FoundItem)
		{
			FoundItem->DestroyItem();
		}
	}

	CurrentWave++;
	if (CurrentWave >= MaxWaves)
	{
		EndLevel();
	}
	else
	{
		StartWaveAfterDelay();
	}
}

void ASpartaGameState::StartWaveAfterDelay()
{
	GetWorldTimerManager().SetTimer(WaveDelayTimerHandle, this,
	                                &ASpartaGameState::StartWave, 5.0f, false);
}

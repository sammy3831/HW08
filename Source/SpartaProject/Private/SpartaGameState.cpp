// Fill out your copyright notice in the Description page of Project Settings.

#include "SpartaGameState.h"
#include "CoinItem.h"
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

	// MaxWaves라는 변수 3을 잡아서 그걸로 웨이브 바꾸기, 웨이브 바꾸는 건 한 레벨에서만 하니 GameInstance 필요없을 듯
	// 변수가 MaxWaves보다 크거나 같다면 EndLevel 사용
	// 현재 30초후거나 코인을 다먹으면 레벨이 바뀌는데 코인을 다먹거나 웨이브시간이 다 되면 웨이브가 바뀌고 마지막 웨이브에서 코인을 다먹거나 시간이 끝나면 레벨 바꾸기
	// LevelDuration을 웨이브시간으로 바꿔도 괜찮을 듯?
	// 현재 레벨마다 총 아이템 40개 스폰. 확률만 에디터에서 다르게 설정해서 스폰중
	// 웨이브마다 스폰개수를 다르게 하기는 확정, 레벨마다 스폰 개수를 다르게 할지 생각
	// 웨이브 1, 2, 3같은 UI나타내야 함 웨이브 끝나면 맵에 스폰되어 있는 모든 아이템 제거, 몇초 후 다음 웨이브 시작(n초 후 다음 웨이브 시작 같은 UI)
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
							ScoreText->SetText(FText::FromString(FString::Printf(TEXT("Score: %d"), SpartaGameInstance->TotalScore)));
						}
					}
				}

				if (UTextBlock* LevelIndexText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Level"))))
				{
					LevelIndexText->SetText(FText::FromString(FString::Printf(TEXT("Level: %d"), CurrentLevelIndex + 1)));
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
						WaveText->SetText(FText::FromString(FString::Printf(TEXT("%.1f초 후 다음 웨이브를 시작합니다!"), RemainingTime)));
					}
				}

				if (UProgressBar* RemainingTimeBar = Cast<UProgressBar>(HUDWidget->GetWidgetFromName(TEXT("RemainingTimeBar"))))
				{
					float RemainingTime = GetWorldTimerManager().GetTimerRemaining(WaveTimerHandle);
					if (RemainingTime < 0.0f)
					{
						RemainingTime = 0.0f;
					}

					float RemainingTimePercent = RemainingTime / WaveDuration; 
					RemainingTimeBar->SetPercent(RemainingTimePercent);
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

// Fill out your copyright notice in the Description page of Project Settings.


#include "STGameMode.h"
#include "TimerManager.h"


ASTGameMode::ASTGameMode()
{
	TimeBetweenWaves = 2.0f;
}

void ASTGameMode::StartWave()
{
	WaveCount++;

	NrOfBotToSpawn = 2 * WaveCount;
	
	GetWorldTimerManager().SetTimer(TimerHandle_BotSpawner, this, &ASTGameMode::SpawnBotTimerElapsed, 1.0f, true, 0.0f);
}

void ASTGameMode::SpawnBotTimerElapsed()
{
	SpawnNewBot();

	NrOfBotToSpawn--;
	
	if (NrOfBotToSpawn <= 0) {
		EndWave();
	}
}

void ASTGameMode::EndWave()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_BotSpawner);
}

void ASTGameMode::PrepareForNextWave()
{
	FTimerHandle TimerHandle_NextWaveStart;
	GetWorldTimerManager().SetTimer(TimerHandle_NextWaveStart, this, &ASTGameMode::StartWave, TimeBetweenWaves, false);
}


void ASTGameMode::StartPlay()
{
	Super::StartPlay();
	
	PrepareForNextWave();
}

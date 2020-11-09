// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "STGameMode.generated.h"

/**
 * 
 */
UCLASS()
class COOPGAME_API ASTGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
protected:

	FTimerHandle TimerHandle_BotSpawner;

	// Bots to spawn in single wave
	int32 NrOfBotToSpawn;

	int32 WaveCount;

	UPROPERTY(EditDefaultsOnly, Category="GameMode")
	float TimeBetweenWaves;

protected:

	// Hook for BP to spawn a single bot
	UFUNCTION(BlueprintImplementableEvent, Category="GameMode")
	void SpawnNewBot();
	
	void SpawnBotTimerElapsed();

	// Start Spawning Bots
	void StartWave();

	// Stop Spawning Bots
	void EndWave();

	// Set timer for next wave
	void PrepareForNextWave();

public:

	ASTGameMode();

	virtual void StartPlay() override;
};

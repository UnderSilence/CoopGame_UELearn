// Fill out your copyright notice in the Description page of Project Settings.


#include "STGameMode.h"
#include "TimerManager.h"
#include "Components/STHealthComponent.h"
#include "EngineUtils.h"
#include "STGameState.h"


ASTGameMode::ASTGameMode()
{
	TimeBetweenWaves = 2.0f;
	WaveCount = 0;
	GameStateClass = ASTGameState::StaticClass();

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 1.0f;
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

	// PrepareForNextWave();
}

void ASTGameMode::PrepareForNextWave()
{
	GetWorldTimerManager().SetTimer(TimerHandle_NextWaveStart, this, &ASTGameMode::StartWave, TimeBetweenWaves, false);
}


void ASTGameMode::CheckWaveState()
{
	bool bIsPrepareingForWave = GetWorldTimerManager().IsTimerActive(TimerHandle_NextWaveStart);

	if (bIsPrepareingForWave || NrOfBotToSpawn > 0) {
		return;
	}

	bool bIsAnyBotAlive = false;
	for (TActorIterator<APawn> It(GetWorld(), APawn::StaticClass()); It; ++It) {
		APawn* TestPawn = *It;

		if (TestPawn == nullptr || TestPawn->IsPlayerControlled()) {
			continue;
		}

		USTHealthComponent* HealthComp = Cast<USTHealthComponent>(TestPawn->GetComponentByClass(USTHealthComponent::StaticClass()));
		if (HealthComp && HealthComp->GetHealth() > 0.0f) {
			bIsAnyBotAlive = true;
			break;
		}
	}

	if (!bIsAnyBotAlive) {
		PrepareForNextWave();
	}
}

void ASTGameMode::CheckAnyPlayerAlive()
{
	for (TActorIterator<APlayerController> It(GetWorld(), APlayerController::StaticClass()); It; ++It) {
		APlayerController* PC = *It;
		if (PC && PC->GetPawn()) {
			APawn* MyPawn = PC->GetPawn();
			USTHealthComponent* HealthComp = Cast<USTHealthComponent>(MyPawn->GetComponentByClass(USTHealthComponent::StaticClass()));

			// ensure will trigger assert if expr is false.
			if (ensure(HealthComp) && HealthComp->GetHealth() > 0.0f) {
				// exist alive player
				return;
			}
		}
	}
	
	// no alive player
	GameOver();
}

void ASTGameMode::GameOver()
{
	EndWave();

	UE_LOG(LogTemp, Log, TEXT("GAME OVER! Players Died"));
}

void ASTGameMode::SetWaveState(EWaveState NewWaveState)
{
	ASTGameState* GS = GetGameState<ASTGameState>();

	if (ensureAlways(GS)) {
		GS->WaveState = NewWaveState;
	}
}

void ASTGameMode::StartPlay()
{
	Super::StartPlay();

	// PrepareForNextWave();
}

void ASTGameMode::Tick(float DeltaSeconds)
{
	CheckWaveState();
	CheckAnyPlayerAlive();
}

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "STGameState.generated.h"

UENUM()
enum class EWaveState : uint8 {

	WaitingToStart,
	WaveInProgress,
	WaitingToComplete,
	WaveComplete,
	GameOver
};



/**
 * 
 */
UCLASS()
class COOPGAME_API ASTGameState : public AGameStateBase
{
	GENERATED_BODY()

protected:

	UFUNCTION()
	void OnRep_WaveState(EWaveState OldWaveState);

	UFUNCTION(BlueprintImplementableEvent, Category="GameState")
	void OnWaveStateChanged(EWaveState NewWaveState, EWaveState OldWaveState);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_WaveState, Category = "GameState")
	EWaveState WaveState;

public:

	void SetWaveState(EWaveState NewWaveState);

};

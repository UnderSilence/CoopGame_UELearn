// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "STPowerupActor.generated.h"

UCLASS()
class COOPGAME_API ASTPowerupActor : public AActor {
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    ASTPowerupActor();

protected:
    // Called when the game starts or when spawned
    // virtual void BeginPlay() override;

    /* Time between powerup ticks*/
    UPROPERTY(EditDefaultsOnly, Category="Powerups")
    float PowerupInterval;

    /* Total times we apply the powerup effect*/
    UPROPERTY(EditDefaultsOnly, Category = "Powerups")
    int32 TotalNrOfTicks;

    int32 TicksProcessed;

    FTimerHandle TimerHandle_PowerupTick;

    UFUNCTION()
    void OnTickPowerup();

    UPROPERTY(Replicated, ReplicatedUsing=OnRep_PowerupActive)
    bool bIsPowerupActive;

    UFUNCTION()
    void OnRep_PowerupActive();

	UFUNCTION(BlueprintImplementableEvent, Category = "Powerups")
    void OnPowerupStateChanged(bool bNewIsActive);

public:
    // Called every frame
    // virtual void Tick(float DeltaTime) override;

    void ActivatePowerup(AActor* ActiveFor);

    UFUNCTION(BlueprintImplementableEvent, Category = "Powerups")
    void OnActivated(AActor* ActiveFor);

    UFUNCTION(BlueprintImplementableEvent, Category = "Powerups")
    void OnPowerupTicked();

    UFUNCTION(BlueprintImplementableEvent, Category = "Powerups")
    void OnExpired();

};

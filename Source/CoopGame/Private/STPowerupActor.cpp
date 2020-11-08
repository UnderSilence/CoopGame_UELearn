// Fill out your copyright notice in the Description page of Project Settings.


#include "STPowerupActor.h"
#include "TimerManager.h"

// Sets default values
ASTPowerupActor::ASTPowerupActor() {
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    // PrimaryActorTick.bCanEverTick = true;
    PowerupInterval = 0.0f;
    TotalNrOfTicks = 0.0f;
    TicksProcessed = 0;
}

// Called when the game starts or when spawned
void ASTPowerupActor::BeginPlay() {
    Super::BeginPlay();

}

void ASTPowerupActor::OnTickPowerup() {
    TicksProcessed++;

    // Implemented by blueprint
    OnPowerupTicked();

    if (TicksProcessed >= TotalNrOfTicks) {
        OnExpired();
        GetWorldTimerManager().ClearTimer(TimerHandle_PowerupTick);
    }
}

void ASTPowerupActor::ActivatePowerup() {
    if (PowerupInterval > 0.0f) {
        GetWorldTimerManager().SetTimer(TimerHandle_PowerupTick, this, &ASTPowerupActor::OnTickPowerup, PowerupInterval, true, 0.0f);
    } else {
        OnTickPowerup();
    }
}


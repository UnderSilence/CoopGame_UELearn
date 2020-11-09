// Fill out your copyright notice in the Description page of Project Settings.


#include "STPowerupActor.h"
#include "TimerManager.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ASTPowerupActor::ASTPowerupActor() {
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    // PrimaryActorTick.bCanEverTick = true;
    PowerupInterval = 0.0f;
    TotalNrOfTicks = 0.0f;
    TicksProcessed = 0;
    bIsPowerupActive = false;

    SetReplicates(true);
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

void ASTPowerupActor::OnRep_PowerupActive()
{
    OnPowerupStateChanged(bIsPowerupActive);
}

void ASTPowerupActor::ActivatePowerup(AActor* ActiveFor) {
    OnActivated(ActiveFor);

    bIsPowerupActive = true;
    // Only run on server
    OnRep_PowerupActive();

    if (PowerupInterval > 0.0f) {
        GetWorldTimerManager().SetTimer(TimerHandle_PowerupTick, this, &ASTPowerupActor::OnTickPowerup, PowerupInterval, true);
    } else {
        OnTickPowerup();
    }
}


void ASTPowerupActor::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// register property which is needed
	DOREPLIFETIME(ASTPowerupActor, bIsPowerupActive);
}

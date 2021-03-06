// Fill out your copyright notice in the Description page of Project Settings.


#include "STGameState.h"
#include "Net/UnrealNetwork.h"

void ASTGameState::OnRep_WaveState(EWaveState OldWaveState)
{
	OnWaveStateChanged(WaveState, OldWaveState);
}


void ASTGameState::SetWaveState(EWaveState NewWaveState)
{
	if (HasAuthority()) {
		
		EWaveState OldWaveState = WaveState;

		WaveState = NewWaveState;
		// Only call on server
		OnRep_WaveState(OldWaveState);
	}
}

void ASTGameState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// register property which is needed
	DOREPLIFETIME(ASTGameState, WaveState);
}

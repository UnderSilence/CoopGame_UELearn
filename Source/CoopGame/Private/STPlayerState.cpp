// Fill out your copyright notice in the Description page of Project Settings.


#include "STPlayerState.h"

void ASTPlayerState::AddScore(float DeltaScore)
{
	float CurrentScore = GetScore();
	SetScore(CurrentScore + DeltaScore);
}

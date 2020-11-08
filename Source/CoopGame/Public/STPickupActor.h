// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "STPickupActor.generated.h"

class USphereComponent;
class UDecalComponent;
class ASTPowerupActor;

UCLASS()
class COOPGAME_API ASTPickupActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASTPickupActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere, Category="Components")
	USphereComponent* SphereComp;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    UDecalComponent* DecalComp;

	UPROPERTY(EditDefaultsOnly, Category = "PickupActors")
	TSubclassOf<ASTPowerupActor> PowerupClass;

    ASTPowerupActor* PowerupInstance;

    UPROPERTY(EditDefaultsOnly, Category = "PickupActors")
	float CooldownDuration;

	void Respawn();

	FTimerHandle TimerHandle_RespawnTimer;

public:	
	// Called every frame
	// virtual void Tick(float DeltaTime) override;
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "STTrackerBot.generated.h"

class UStaticMeshComponent;
class USTHealthComponent;
class ParticleSystem;
class USphereComponent;
class USoundCue;

UCLASS()
class COOPGAME_API ASTTrackerBot : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ASTTrackerBot();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleDefaultsOnly, Category="Components")
	UStaticMeshComponent* MeshComp; 

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Components")
	USTHealthComponent* HealthComp;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	USphereComponent* SphereComp;

    UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	USphereComponent* AllianceSphereComp;

	// Remember to set binding function to UFUNTION()
	UFUNCTION()
	void HandleTakeDamage(USTHealthComponent* ThisHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType,
		class AController* InstigatedBy, AActor* DamageCauser);

	FVector GetNextPathPoint();

	FVector NextPathPoint;

	UPROPERTY(EditDefaultsOnly, Category="TrackerBot")
	float MovementForce;

	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
	bool bUseVelocityChange;

	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
	float RequiredDistanceToTarget;

	// Dynamic material to pulse
	UMaterialInstanceDynamic* MatInst;

	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
	UParticleSystem* ExplosionEffect;

	void SelfDestruct();

	bool bExploded;

	bool bStartSelfDestruction;

	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
	float ExplosionRadius;

	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
	float DefaultExplosionDamage;

	float ExplosionDamage;

	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
	float SelfDamageInterval;

	FTimerHandle TimerHandle_SelfDamage;

	FTimerHandle TimerHandle_SearchAlliance;

	void DamageSelf();

	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
	USoundCue* ExplosionSound;

	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
	USoundCue* SelfDestructionSound;

    UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
	int32 MaxPowerLevel;
	
	int32 CurrentPowerLevel;

	void SearchAlliance();

	UFUNCTION()
	void OnTriggerSelfDestruct(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	// bool bStopped;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
};

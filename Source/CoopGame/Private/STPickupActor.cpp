// Fill out your copyright notice in the Description page of Project Settings.


#include "STPickupActor.h"
#include <Components/SphereComponent.h>
#include <Components/DecalComponent.h>
#include "STPowerupActor.h"

// Sets default values
ASTPickupActor::ASTPickupActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	// PrimaryActorTick.bCanEverTick = true;

	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SphereComp->SetSphereRadius(75.0f);
    SphereComp->SetCollisionResponseToAllChannels(ECR_Ignore);
    SphereComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	RootComponent = SphereComp;

	DecalComp = CreateDefaultSubobject<UDecalComponent>(TEXT("DecalComp"));
	DecalComp->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f));
	DecalComp->DecalSize = FVector(64.0f, 75.0f, 75.0f);
	DecalComp->SetupAttachment(RootComponent);

	CooldownDuration = 5.0f;

	SetReplicates(true);
}

// Called when the game starts or when spawned
void ASTPickupActor::BeginPlay()
{
	Super::BeginPlay();
	if (HasAuthority()) {
		Respawn();
	}
}

void ASTPickupActor::Respawn() {
	if (PowerupClass == nullptr) {
		UE_LOG(LogTemp, Warning, TEXT("PowerupClass is nullptr in %s, Please update your Blueprint"), *GetName());
		return;
	}

	// Initialize spawn parameters
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	PowerupInstance = GetWorld()->SpawnActor<ASTPowerupActor>(PowerupClass, GetTransform(), SpawnParams);
}

void ASTPickupActor::NotifyActorBeginOverlap(AActor* OtherActor) {
	// Make sure to call base function
	Super::NotifyActorBeginOverlap(OtherActor);
	
	// @TODO: Grant a powerup to player if available
	if (HasAuthority() && PowerupInstance) {
		PowerupInstance->ActivatePowerup(OtherActor);
		// Player pickup the PowerupInstance
		PowerupInstance = nullptr;

		// Set Timer to respawn
		GetWorldTimerManager().SetTimer(TimerHandle_RespawnTimer, this, &ASTPickupActor::Respawn, CooldownDuration, false);
	}
}

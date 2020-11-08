// Fill out your copyright notice in the Description page of Project Settings.


#include "STPickupActor.h"
#include <Components/SphereComponent.h>
#include <Components/DecalComponent.h>

// Sets default values
ASTPickupActor::ASTPickupActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	// PrimaryActorTick.bCanEverTick = true;

	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SphereComp->SetSphereRadius(75.0f);
	RootComponent = SphereComp;

	DecalComp = CreateDefaultSubobject<UDecalComponent>(TEXT("DecalComp"));
	DecalComp->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f));
	DecalComp->DecalSize = FVector(64.0f, 75.0f, 75.0f);
	DecalComp->SetupAttachment(RootComponent);

}

// Called when the game starts or when spawned
void ASTPickupActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASTPickupActor::NotifyActorBeginOverlap(AActor* OtherActor) {
	// Make sure to call base function
	Super::NotifyActorBeginOverlap(OtherActor);
	
	// @TODO: Grant a powerup to player if available

}

/*
// Called every frame
void ASTPickupActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
*/

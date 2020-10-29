// Fill out your copyright notice in the Description page of Project Settings.

#include "STGrenade.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "DrawDebugHelpers.h"

// Sets default values
ASTGrenade::ASTGrenade()
{
	// Initialize Sphere collision
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &ASTGrenade::OnHit);

	RootComponent = CollisionComp;

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 1000.f;
	ProjectileMovement->MaxSpeed = 1500.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;

	InitialLifeSpan = 1.0f;
}

void ASTGrenade::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if ((OtherActor != NULL) && (OtherActor != this) && (OtherComp != NULL))
	{
		DrawDebugSphere(GetWorld(), GetActorLocation(), 20.0f, 20, FColor::Red, false, 1.0f, 1.0f, 1.0f);
		// OtherComp->AddImpulseAtLocation(GetVelocity() * 100.0f, GetActorLocation());
		return;
	}

	MakeNoise(1.0f, GetInstigator());
	Destroy();
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "STGrenadeGun.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "STGrenade.h"

ASTGrenadeGun::ASTGrenadeGun()
{
	MuzzleSocketName = "MuzzleFlashSocket";
}

void ASTGrenadeGun::Fire()
{

	AActor* MyOwner = GetOwner();
	if (MyOwner && ProjectileClass) {
		FVector EyeLocation;
		FRotator EyeRotation;
		MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

		FVector ShotDirection = EyeRotation.Vector();
		FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);
		// DrawDebugLine(GetWorld(), EyeLocation, TraceEnd, FColor::White, false, 1.0f, 0, 1.0f);

		//Set Spawn Collision Handling Override
		FActorSpawnParameters ActorSpawnParams;
		ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
		ActorSpawnParams.Instigator = Cast<APawn>(MyOwner);

		if (MuzzleEffect) {
			UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, MeshComp, MuzzleSocketName);
		}
		
		GetWorld()->SpawnActor<ASTGrenade>(ProjectileClass, MuzzleLocation, EyeRotation, ActorSpawnParams);
	}
}

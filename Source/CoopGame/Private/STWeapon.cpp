// Fill out your copyright notice in the Description page of Project Settings.

#include "STWeapon.h"
#include "DrawDebugHelpers.h"
#include "kismet/GameplayStatics.h"
#include "Components/SkeletalMeshComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "TimerManager.h"
#include "../CoopGame.h"
#include "Net/UnrealNetwork.h"

int32 DebugWeaponDrawing = 0;
FAutoConsoleVariableRef CVARDebugWeaponDrawing(
	TEXT("COOP.DebugWeapons"),
	DebugWeaponDrawing,
	TEXT("Draw Debug lines for Weapons"),
	ECVF_Cheat);


// Sets default values
ASTWeapon::ASTWeapon()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;

	MuzzleSocketName = "MuzzleSocket";
	TracerTargetName = "Target";

	BaseDamage = 20.0f;

	// RPM - bullets per minutes
	RateOfFire = 600.0f;

	SetReplicates(true);

	NetUpdateFrequency = 66.0f;
	MinNetUpdateFrequency = 33.0f;
}


void ASTWeapon::BeginPlay()
{
	Super::BeginPlay();
	
	TimeBetweenShots = 60 / RateOfFire;
}

// Fire function need to be replicated, like RPC
void ASTWeapon::Fire()
{
	// Trace the world from pawn eye to cross hair location
	if (GetLocalRole() < ROLE_Authority) {
		ServerFire();
	}

	AActor* MyOwner = GetOwner();
	if (MyOwner) {
		FVector EyeLocation;
		FRotator EyeRotation;
		MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

		FVector ShotDirection = EyeRotation.Vector();
		FVector TraceEnd = EyeLocation + (EyeRotation.Vector() * 10000.0f);

		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(MyOwner);
		QueryParams.AddIgnoredActor(this);
		QueryParams.bTraceComplex = true;
		QueryParams.bReturnPhysicalMaterial = true;

		FVector TracerEndPoint = TraceEnd;
		EPhysicalSurface SurfaceType = SurfaceType_Default;

		FHitResult Hit;
		if (GetWorld()->LineTraceSingleByChannel(Hit, EyeLocation, TraceEnd, COLLISION_WEAPON, QueryParams)) {
			// Blocking hit! Process damage

			AActor* HitActor = Hit.GetActor();

			SurfaceType = UPhysicalMaterial::DetermineSurfaceType(Hit.PhysMaterial.Get());


			float ActualDamage = BaseDamage;
			if (SurfaceType == SURFACE_FLESHVULNERABLE) {
				ActualDamage *= 10.0f;
			}

			UGameplayStatics::ApplyPointDamage(HitActor, ActualDamage, ShotDirection, Hit, MyOwner->GetInstigatorController(), this, DamageType);
			DrawDebugSphere(GetWorld(), Hit.ImpactPoint, 10, 12, FColor::Yellow, false, 1.0f, 0, 1.0f);

			PlayImpactEffects(SurfaceType, Hit.ImpactPoint);

			TracerEndPoint = Hit.ImpactPoint;
		}
		if (DebugWeaponDrawing > 0) {
			DrawDebugLine(GetWorld(), EyeLocation, TraceEnd, FColor::White, false, TimeBetweenShots, 0, 1.0f);
		}

		PlayFireEffects(TracerEndPoint);

		if (HasAuthority()) {
			HitScanTrace.TraceTo = TracerEndPoint;
			HitScanTrace.SurfaceType = SurfaceType;
		}

		LastFireTime = GetWorld()->GetTimeSeconds();
	}
}


void ASTWeapon::OnRep_HitScanTrace()
{
	// Play cosmetic 
	PlayFireEffects(HitScanTrace.TraceTo);
	PlayImpactEffects(HitScanTrace.SurfaceType, HitScanTrace.TraceTo);
}

void ASTWeapon::ServerFire_Implementation()
{
	Fire();
}

bool ASTWeapon::ServerFire_Validate()
{
	return true;
}

void ASTWeapon::StartFire()
{
	float FirstDelay = FMath::Max(0.0f, TimeBetweenShots - GetWorld()->TimeSeconds);

	GetWorldTimerManager().SetTimer(TimerHandle_TimeBetweenShots, this, &ASTWeapon::Fire, TimeBetweenShots, true, FirstDelay);
}

void ASTWeapon::StopFire()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_TimeBetweenShots);
}

void ASTWeapon::PlayFireEffects(FVector TracerEndPoint) {
	if (MuzzleEffect) {
		UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, MeshComp, MuzzleSocketName);
	}

	if (TracerEffect) {
		FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);

		UParticleSystemComponent* TracerComp = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TracerEffect, MuzzleLocation);
		if (TracerComp) {
			TracerComp->SetVectorParameter(TracerTargetName, TracerEndPoint);
		}
	}

	auto MyOwnerPawn = Cast<APawn>(GetOwner());
	if (MyOwnerPawn) {
		auto PC = Cast<APlayerController>(MyOwnerPawn->GetController());
		if (PC) {
			PC->ClientPlayCameraShake(FireCamShake);
		}
	}
}


void ASTWeapon::PlayImpactEffects(EPhysicalSurface SurfaceType, FVector ImpactPoint)
{
	UParticleSystem* SelectedImpactEffect = nullptr;
	switch (SurfaceType)
	{
	case SURFACE_FLESHDEFAULT:
	case SURFACE_FLESHVULNERABLE:
		SelectedImpactEffect = FleshImpactEffect;
		break;
	default:
		SelectedImpactEffect = DefaultImpactEffect;
		break;
	}

	if (SelectedImpactEffect) {
		FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);
		FVector ShotDirection = ImpactPoint - MuzzleLocation;
		ShotDirection.Normalize();

		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SelectedImpactEffect, ImpactPoint, ShotDirection.Rotation());
	}
}

void ASTWeapon::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// register property which is needed
	DOREPLIFETIME_CONDITION(ASTWeapon, HitScanTrace, COND_SkipOwner);
}

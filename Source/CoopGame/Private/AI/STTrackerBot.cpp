// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/STTrackerBot.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "DrawDebugHelpers.h"
#include "Components/STHealthComponent.h"
#include "Particles/ParticleSystem.h"
#include "Components/SphereComponent.h"
#include "STCharacter.h"
#include "Sound/SoundCue.h"
#include "TimerManager.h"

// Sets default values
ASTTrackerBot::ASTTrackerBot()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetCanEverAffectNavigation(false);
	MeshComp->SetSimulatePhysics(true);
	RootComponent = MeshComp;

	HealthComp = CreateDefaultSubobject<USTHealthComponent>(TEXT("HealthComp"));
	HealthComp->OnHealthChanged.AddDynamic(this, &ASTTrackerBot::HandleTakeDamage);

	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SphereComp->SetSphereRadius(200);
	SphereComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	SphereComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	SphereComp->SetupAttachment(RootComponent);
	SphereComp->OnComponentBeginOverlap.AddDynamic(this, &ASTTrackerBot::OnTriggerSelfDestruct);

    AllianceSphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("AllianceSphereComp")); 
	AllianceSphereComp->SetSphereRadius(500);
    AllianceSphereComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    AllianceSphereComp->SetCollisionResponseToAllChannels(ECR_Ignore);
    AllianceSphereComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	AllianceSphereComp->SetupAttachment(RootComponent);

	MovementForce = 1000.0f;
	bUseVelocityChange = false;
	RequiredDistanceToTarget = 100;
	DefaultExplosionDamage = 80.0f;
	ExplosionRadius = 200.0f;
	SelfDamageInterval = 0.25f;
	bExploded = false;
	bStartSelfDestruction = false;
	
	// Challenge Code
	MaxPowerLevel = 3;
	CurrentPowerLevel = 0;
	ExplosionDamage = DefaultExplosionDamage;
}

// Called when the game starts or when spawned
void ASTTrackerBot::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority()) {
		// find initial target
        NextPathPoint = GetNextPathPoint();
        GetWorldTimerManager().SetTimer(TimerHandle_SearchAlliance, this, &ASTTrackerBot::SearchAlliance, 1.0f, true, 0.0f);
	}
	
}

void ASTTrackerBot::HandleTakeDamage(USTHealthComponent* ThisHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{

	// @TODO: Pulse the material on hit

	if (MatInst == nullptr) {
		MatInst = MeshComp->CreateAndSetMaterialInstanceDynamicFromMaterial(0, MeshComp->GetMaterial(0));
	}
	else {
		float CurrentTime = GetWorld()->TimeSeconds;
		MatInst->SetScalarParameterValue("LastTimeDamageTaken", CurrentTime);
	}

	UE_LOG(LogTemp, Log, TEXT("Health %s of %s"), *FString::SanitizeFloat(Health), *GetName());

	// Explode on hitpoints == 0
	if (Health <= 0.0f) {
		SelfDestruct();
	}
}

FVector ASTTrackerBot::GetNextPathPoint()
{
	// To access PlayerPawn's location
	ACharacter* PlayerPawn = UGameplayStatics::GetPlayerCharacter(this, 0);

	UNavigationPath* NavPath = UNavigationSystemV1::FindPathToActorSynchronously(this, GetActorLocation(), PlayerPawn);

	if (NavPath && NavPath->PathPoints.Num() > 1) {
		// Return next point in the path
		return NavPath->PathPoints[1];
	}

	// Failed to find path
	return GetActorLocation();
}

void ASTTrackerBot::SelfDestruct()
{

	if (bExploded) {
		return;
	}

	bExploded = true;
	// Spawn Explosion particles
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, GetActorLocation());

	UGameplayStatics::PlaySoundAtLocation(this, ExplosionSound, GetActorLocation());
	
    MeshComp->SetVisibility(false, true);
	MeshComp->SetSimulatePhysics(false);
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (HasAuthority()) {
		TArray<AActor*> IgnoredActors;
		IgnoredActors.Add(this);

		// Apply damage
		UGameplayStatics::ApplyRadialDamage(this, ExplosionDamage, GetActorLocation(), ExplosionRadius, nullptr, IgnoredActors, this, GetInstigatorController(), true);
		DrawDebugSphere(GetWorld(), GetActorLocation(), ExplosionRadius, 20, FColor::Red, false, 2.0f, 0, 1.0f);

		SetLifeSpan(2.0f);
	}
}

void ASTTrackerBot::DamageSelf()
{
	UGameplayStatics::ApplyDamage(this, 20, GetInstigatorController(), this, nullptr);
}

void ASTTrackerBot::SearchAlliance() {
	TSet<AActor*> Neighbers;
	AllianceSphereComp->GetOverlappingActors(Neighbers, ASTTrackerBot::StaticClass());
	DrawDebugSphere(GetWorld(), GetActorLocation(), AllianceSphereComp->GetScaledSphereRadius(), 20, FColor::White, false, 1.0f, 0, 1.0f);
	CurrentPowerLevel = FMath::Min(MaxPowerLevel, FMath::Max(0, Neighbers.Num()-1));
	float PowerLevelAlpha = CurrentPowerLevel / (float)MaxPowerLevel;

    if (MatInst == nullptr) {
        MatInst = MeshComp->CreateAndSetMaterialInstanceDynamicFromMaterial(0, MeshComp->GetMaterial(0));
    }
	// Update Materials
	MatInst->SetScalarParameterValue("PowerLevelAlpha", PowerLevelAlpha);
	// Update Power-ups Damage
	ExplosionDamage = (PowerLevelAlpha + 1.0f) * DefaultExplosionDamage;
}

void ASTTrackerBot::OnTriggerSelfDestruct(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
    if (!bStartSelfDestruction && !bExploded) {
        ASTCharacter* PlayerPawn = Cast<ASTCharacter>(OtherActor);
        if (PlayerPawn) {
            if (HasAuthority()) {
                // Start self destruction sequence
                GetWorldTimerManager().SetTimer(TimerHandle_SelfDamage, this, &ASTTrackerBot::DamageSelf, SelfDamageInterval, true, 0.0f);
            }
            bStartSelfDestruction = true;

            UGameplayStatics::SpawnSoundAttached(SelfDestructionSound, RootComponent);
        }
    }
}

// Called every frame
void ASTTrackerBot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Tick only on server
	if (HasAuthority() && !bExploded) {
		float DistanceToTarget = (GetActorLocation() - NextPathPoint).Size();

		if (DistanceToTarget <= RequiredDistanceToTarget) {
			NextPathPoint = GetNextPathPoint();
			// DrawDebugString(GetWorld(), GetActorLocation(), "Reached Target!", 0, FColor::Yellow, 1.0f, false, 1.0f);
		}
		else {
			// Keep moving towards next target
			FVector ForceDirection = NextPathPoint - GetActorLocation();
			ForceDirection.Normalize();

			MeshComp->AddForce(ForceDirection * MovementForce, NAME_None, bUseVelocityChange);
			DrawDebugDirectionalArrow(GetWorld(), GetActorLocation(), GetActorLocation() + ForceDirection * MovementForce, 64, FColor::Yellow, false, 0.0f, 0, 1.0f);
		}

		DrawDebugSphere(GetWorld(), NextPathPoint, 20, 12, FColor::Yellow, false, 0.0f, 0, 1.0f);
	}
}


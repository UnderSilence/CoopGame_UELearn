// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/STTrackerBot.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "DrawDebugHelpers.h"
#include "Components/STHealthComponent.h"

// Sets default values
ASTTrackerBot::ASTTrackerBot()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetCanEverAffectNavigation(false);
	MeshComp->SetSimulatePhysics(true);
	
	HealthComp = CreateDefaultSubobject<USTHealthComponent>(TEXT("HealthComp"));
	HealthComp->OnHealthChanged.AddDynamic(this, &ASTTrackerBot::HandleTakeDamage);

	RootComponent = MeshComp;

	MovementForce = 1000.0f;
	bUseVelocityChange = false;
	RequiredDistanceToTarget = 100;
}

// Called when the game starts or when spawned
void ASTTrackerBot::BeginPlay()
{
	Super::BeginPlay();

	// find initial target
	NextPathPoint = GetNextPathPoint();
}

void ASTTrackerBot::HandleTakeDamage(USTHealthComponent* ThisHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	// Explode on hitpoints == 0

	// @TODO: Pulse the material on hit
	UE_LOG(LogTemp, Log, TEXT("Health %s of %s"), *FString::SanitizeFloat(Health), *GetName());
}

FVector ASTTrackerBot::GetNextPathPoint()
{
	// To access PlayerPawn's location
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);

	UNavigationPath* NavPath = UNavigationSystemV1::FindPathToActorSynchronously(this, GetActorLocation(), PlayerPawn);

	if (NavPath->PathPoints.Num() > 1) {
		// Return next point in the path
		return NavPath->PathPoints[1];
	}

	// Failed to find path
	return GetActorLocation();
}

// Called every frame
void ASTTrackerBot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float DistanceToTarget = (GetActorLocation() - NextPathPoint).Size();

	if (DistanceToTarget <= RequiredDistanceToTarget) {
		NextPathPoint = GetNextPathPoint();
		DrawDebugString(GetWorld(), GetActorLocation(), "Reached Target!", 0, FColor::Yellow, 1.0f, false, 1.0f);
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



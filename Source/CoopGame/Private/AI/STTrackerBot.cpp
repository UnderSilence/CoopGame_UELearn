// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/STTrackerBot.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"

// Sets default values
ASTTrackerBot::ASTTrackerBot()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetCanEverAffectNavigation(false);
	
	RootComponent = MeshComp;
}

// Called when the game starts or when spawned
void ASTTrackerBot::BeginPlay()
{
	Super::BeginPlay();
	
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

}



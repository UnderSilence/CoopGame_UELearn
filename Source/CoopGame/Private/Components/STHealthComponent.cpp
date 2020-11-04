// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/STHealthComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
USTHealthComponent::USTHealthComponent()
{
	DefaultHealthPoint = 100.0f;

	// SetIsReplicated(true);
}


// Called when the game starts
void USTHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	// Only hook if we are server
	if (GetOwnerRole() == ROLE_Authority) {
		AActor* MyOwner = GetOwner();
		if (MyOwner) {
			MyOwner->OnTakeAnyDamage.AddDynamic(this, &USTHealthComponent::HandleTakeAnyDamage);
		}
	}

	HealthPoint = DefaultHealthPoint;
}

void USTHealthComponent::HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (Damage <= 0.0f) {
		return;
	}

	HealthPoint = FMath::Clamp(HealthPoint - Damage, 0.0f, DefaultHealthPoint);

	UE_LOG(LogTemp, Log, TEXT("Health Changed: %s"), *FString::SanitizeFloat(HealthPoint));

	OnHealthChanged.Broadcast(this, HealthPoint, Damage, DamageType, InstigatedBy, DamageCauser);
}


void USTHealthComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// register property which is needed
	DOREPLIFETIME(USTHealthComponent, HealthPoint);
}
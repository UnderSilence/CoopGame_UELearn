// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/STHealthComponent.h"
#include "Net/UnrealNetwork.h"
#include "STGameMode.h"

// Sets default values for this component's properties
USTHealthComponent::USTHealthComponent()
{
	DefaultHealthPoint = 100.0f;

	bIsDead = false;
	// 设置多人游戏客户端与服务端数据同步，非常重要
	SetIsReplicated(true);
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

// 留给Client调用的，血量变化回调，让Client可以触发血量变化事件从而同步状态。
void USTHealthComponent::OnRep_Health(float OldHealthPoint)
{
	float Damage = OldHealthPoint - HealthPoint;
	OnHealthChanged.Broadcast(this, HealthPoint, Damage, nullptr, nullptr, nullptr);
}

void USTHealthComponent::HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (Damage <= 0.0f || bIsDead) {
		return;
	}

	HealthPoint = FMath::Clamp(HealthPoint - Damage, 0.0f, DefaultHealthPoint);

	UE_LOG(LogTemp, Log, TEXT("Health Changed: %s"), *FString::SanitizeFloat(HealthPoint));

	OnHealthChanged.Broadcast(this, HealthPoint, Damage, DamageType, InstigatedBy, DamageCauser);

	bIsDead = HealthPoint <= 0.0f;

	if (bIsDead) {
		ASTGameMode* GM = Cast<ASTGameMode>(GetWorld()->GetAuthGameMode());
		if (GM) {
			GM->OnActorKilled.Broadcast(GetOwner(), DamageCauser, InstigatedBy);
		}
	}
}

void USTHealthComponent::Heal(float HealAmount)
{
	if (HealAmount <= 0.0f || HealthPoint <= 0.0f) {
		return;
	}
	
	HealthPoint = FMath::Min(HealthPoint + HealAmount, DefaultHealthPoint);

	UE_LOG(LogTemp, Log, TEXT("Health Changed: %s (+%s)"), *FString::SanitizeFloat(HealthPoint), *FString::SanitizeFloat(HealAmount));
	
	OnHealthChanged.Broadcast(this, HealthPoint, -HealAmount, nullptr, nullptr, nullptr);
}


float USTHealthComponent::GetHealth() const
{
	return HealthPoint;
}

void USTHealthComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// register property which is needed
	DOREPLIFETIME(USTHealthComponent, HealthPoint);
}

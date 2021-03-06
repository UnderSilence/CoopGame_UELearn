// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "STHealthComponent.generated.h"

// OnHealthChanged event
DECLARE_DYNAMIC_MULTICAST_DELEGATE_SixParams(FOnHealthChangedSignature, USTHealthComponent*, HealthComp, float, Health, float, HealthDelta, const class UDamageType*, DamageType, class AController*, InstigatedBy, AActor*, DamageCauser);


UCLASS( ClassGroup=(COOP), meta=(BlueprintSpawnableComponent) )
class COOPGAME_API USTHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USTHealthComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	bool bIsDead;
	
	UPROPERTY(Replicated, ReplicatedUsing=OnRep_Health, BlueprintReadOnly, Category="HealthComponent")
	float HealthPoint;

	UFUNCTION()
	void OnRep_Health(float OldHealthPoint);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HealthComponent")
	float DefaultHealthPoint;

	UFUNCTION()
	void HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

public:	
	
	UPROPERTY(BlueprintAssignable, Category="Events")
	FOnHealthChangedSignature OnHealthChanged;

	UFUNCTION(BlueprintCallable, Category="HealthComponent")
	void Heal(float HealAmount);
	
	float GetHealth() const;
};

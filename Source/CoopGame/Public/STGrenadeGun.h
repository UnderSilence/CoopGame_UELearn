// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "STWeapon.h"
#include "STGrenadeGun.generated.h"

class ASTGrenade;

UCLASS()
class COOPGAME_API ASTGrenadeGun : public ASTWeapon
{
	GENERATED_BODY()

protected:
	virtual void Fire() override;

public:
	UPROPERTY(EditDefaultsOnly, Category="Projectile")
	TSubclassOf<ASTGrenade> ProjectileClass;

};

// Fill out your copyright notice in the Description page of Project Settings.


#include "STCharacter.h"
#include "STWeapon.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "../CoopGame.h"
#include "Components/CapsuleComponent.h"

// Sets default values
ASTCharacter::ASTCharacter() {
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComp->bUsePawnControlRotation = true;
	SpringArmComp->SetupAttachment(RootComponent);

	//Check Allowed To Crouch
	GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;
	GetMovementComponent()->GetNavAgentPropertiesRef().bCanJump = true;

	GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Ignore);

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp);

	ZoomedFOV = 60;

	WeaponAttachSocketName = "WeaponSocket";
}

// Called when the game starts or when spawned
void ASTCharacter::BeginPlay() {
	Super::BeginPlay();

	DefaultFOV = CameraComp->FieldOfView;

	if (StaterWeaponClass) {
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		CurrentWeapon = GetWorld()->SpawnActor<ASTWeapon>(StaterWeaponClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);

		if (CurrentWeapon) {
			CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponAttachSocketName);
			CurrentWeapon->SetOwner(this);
		}
	}
}

void ASTCharacter::MoveForward(float Value) {
	AddMovementInput(GetActorForwardVector() * Value);
}

void ASTCharacter::MoveRight(float Value) {
	AddMovementInput(GetActorRightVector() * Value);
}

void ASTCharacter::BeginCrouch() {
	Crouch();
}

void ASTCharacter::EndCrouch() {
	UnCrouch();
}

void ASTCharacter::BeginZoom() {
	bWantsZoom = true;
}

void ASTCharacter::EndZoom() {
	bWantsZoom = false;
}

void ASTCharacter::StartFire()
{
	if (CurrentWeapon) {
		CurrentWeapon->StartFire();
	}
}

void ASTCharacter::StopFire()
{
	if (CurrentWeapon) {
		CurrentWeapon->StopFire();
	}
}

// Called every frame
void ASTCharacter::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	float TargetFOV = bWantsZoom ? ZoomedFOV : DefaultFOV;

	float NewFOV = FMath::FInterpTo(CameraComp->FieldOfView, TargetFOV, DeltaTime, ZoomInterpSpeed);

	CameraComp->SetFieldOfView(NewFOV);
}

// Called to bind functionality to input
void ASTCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ASTCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASTCharacter::MoveRight);

	PlayerInputComponent->BindAxis("LookUp", this, &ASTCharacter::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Turn", this, &ASTCharacter::AddControllerYawInput);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ASTCharacter::BeginCrouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &ASTCharacter::EndCrouch);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ASTCharacter::Jump);

	PlayerInputComponent->BindAction("Zoom", IE_Pressed, this, &ASTCharacter::BeginZoom);
	PlayerInputComponent->BindAction("Zoom", IE_Released, this, &ASTCharacter::EndZoom);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ASTCharacter::StartFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ASTCharacter::StopFire);
}

FVector ASTCharacter::GetPawnViewLocation() const {
	if (CameraComp) {
		return CameraComp->GetComponentLocation();
	}

	// fall back
	return Super::GetPawnViewLocation();
}



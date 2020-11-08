// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include <ProceduralMeshComponent.h>
#include "ViewFrustum.generated.h"

UCLASS()
class COOPGAME_API AViewFrustum : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AViewFrustum();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
    virtual void PostActorCreated() override;
    virtual void PostLoad() override;
    virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Field Props")
    float Fovy;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Field Props")
    float AspectRatio;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Field Props")
    float ZNear;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Field Props")
    float ZFar;

    TArray<FVector> Vertices;
    TArray<int32> Triangles;
    TArray<FVector> Normals;
    TArray<FProcMeshTangent> Tangents;
    TArray<FVector2D> UV0;
    TArray<FColor> VertexColors;

    UProceduralMeshComponent* ProcMeshComp;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Field Props")
    UMaterial* GridFieldMaterial;
    UMaterialInstanceDynamic* GridFieldMaterialInst;

    void AddTriangleMesh(FVector A, FVector B, FVector C, int32& TriIndex, FProcMeshTangent Tangent);
    void AddQuadMesh(FVector A, FVector B, FVector C, FVector D, int32& TriIndex, FProcMeshTangent Tangent);
    void ClearMeshData();

public:	
	// Called every frame
	// virtual void Tick(float DeltaTime) override;
    void CreateViewFrustum();
    void UpdateViewFrustum();
};

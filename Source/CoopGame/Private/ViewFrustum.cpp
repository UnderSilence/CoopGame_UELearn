// Fill out your copyright notice in the Description page of Project Settings.


#include "ViewFrustum.h"
#include <ProceduralMeshComponent.h>
#include <Components/LineBatchComponent.h>

// Sets default values
AViewFrustum::AViewFrustum()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
    GridFieldMaterial = ConstructorHelpers::FObjectFinderOptional<UMaterial>(TEXT("Material'/Game/VisualFields/M_ViewFrustum.M_ViewFrustum'")).Get();
    // GridFieldMaterialInst = UMaterialInstanceDynamic::Create(GridFieldMaterial, ProcMeshComp);

    ProcMeshComp = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("ProcMeshComp"));
    ProcMeshComp->CastShadow = false;
    RootComponent = ProcMeshComp;

    Fovy = 60.0f;
    AspectRatio = 1.5f;
    ZNear = 0.0f;
    ZFar = 500.0f;
}

// Called when the game starts or when spawned
void AViewFrustum::BeginPlay()
{
	Super::BeginPlay();

}

void AViewFrustum::PostActorCreated() {
    Super::PostActorCreated();
    CreateViewFrustum();
}

void AViewFrustum::PostLoad() {
    Super::PostLoad();
    CreateViewFrustum();
}

void AViewFrustum::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) {
    Super::PostEditChangeProperty(PropertyChangedEvent);
    if (PropertyChangedEvent.Property != nullptr) {
        const FName PropertyName = PropertyChangedEvent.Property->GetFName();
        UpdateViewFrustum();
        // DrawDebugString(GetWorld(), GetActorLocation(), PropertyName.ToString(), NULL, FColor::Blue, 2.0f, false, 5.0f);
    }
}


void AViewFrustum::CreateViewFrustum() {
    ClearMeshData();
    float TangentOfHalfFovy = FMath::Tan(FMath::DegreesToRadians(Fovy * 0.5f));
    float ViewHeightFar = ZFar * TangentOfHalfFovy;
    float ViewWidthFar = ViewHeightFar * AspectRatio;
    float ViewHeightNear = ZNear * TangentOfHalfFovy;
    float ViewWidthNear = ViewHeightNear * AspectRatio;

    FVector Origin = FVector::ZeroVector;
    FVector AFar = Origin + FVector(-ViewWidthFar, ViewHeightFar, ZFar);
    FVector BFar = Origin + FVector(ViewWidthFar, ViewHeightFar, ZFar);
    FVector CFar = Origin + FVector(ViewWidthFar, -ViewHeightFar, ZFar);
    FVector DFar = Origin + FVector(-ViewWidthFar, -ViewHeightFar, ZFar);

    FVector ANear = Origin + FVector(-ViewWidthNear, ViewHeightNear, ZNear);
    FVector BNear = Origin + FVector(ViewWidthNear, ViewHeightNear, ZNear);
    FVector CNear = Origin + FVector(ViewWidthNear, -ViewHeightNear, ZNear);
    FVector DNear = Origin + FVector(-ViewWidthNear, -ViewHeightNear, ZNear);
    int32 TrianglesCount = 0;
    FProcMeshTangent TangentSetup = FProcMeshTangent(0.f, 1.f, 0.f);

    if (ZNear < 0.5f) {
        AddQuadMesh(AFar, BFar, CFar, DFar, TrianglesCount, TangentSetup);
        AddTriangleMesh(Origin, BFar, AFar, TrianglesCount, TangentSetup);
        AddTriangleMesh(Origin, CFar, BFar, TrianglesCount, TangentSetup);
        AddTriangleMesh(Origin, DFar, CFar, TrianglesCount, TangentSetup);
        AddTriangleMesh(Origin, AFar, DFar, TrianglesCount, TangentSetup);
    } else {
        AddQuadMesh(AFar, BFar, CFar, DFar, TrianglesCount, TangentSetup);
        AddQuadMesh(ANear, DNear, CNear, BNear, TrianglesCount, TangentSetup);
        AddQuadMesh(ANear, BNear, BFar, AFar, TrianglesCount, TangentSetup);
        AddQuadMesh(BNear, CNear, CFar, BFar, TrianglesCount, TangentSetup);
        AddQuadMesh(CNear, DNear, DFar, CFar, TrianglesCount, TangentSetup);
        AddQuadMesh(DNear, ANear, AFar, DFar, TrianglesCount, TangentSetup);
    }

    ProcMeshComp->CreateMeshSection(0, Vertices, Triangles, Normals, UV0, UV0, UV0, UV0, VertexColors, Tangents, false);
    GridFieldMaterialInst = ProcMeshComp->CreateAndSetMaterialInstanceDynamicFromMaterial(0, GridFieldMaterial);

    ProcMeshComp->SetMaterial(0, GridFieldMaterialInst);

    /*
    TArray<FBatchedLine> Guidelines;
    Guidelines.Add(FBatchedLine(Origin, AFar, FLinearColor(1, 0, 0), 0, 0.3, 0));
    Guidelines.Add(FBatchedLine(Origin, BFar, FLinearColor(1, 0, 0), 0, 0.3, 0));
    Guidelines.Add(FBatchedLine(Origin, CFar, FLinearColor(1, 0, 0), 0, 0.3, 0));
    Guidelines.Add(FBatchedLine(Origin, DFar, FLinearColor(1, 0, 0), 0, 0.3, 0));
    ULineBatchComponent* const LineBatchComponent = GetWorld()->PersistentLineBatcher;
    LineBatchComponent->BatchedLines.Empty();
    LineBatchComponent->DrawLines(Guidelines);
    */
}

void AViewFrustum::UpdateViewFrustum() {
    CreateViewFrustum();
}

void AViewFrustum::AddTriangleMesh(FVector A, FVector B, FVector C, int32& TriIndex, FProcMeshTangent Tangent) {
    int Point1 = TriIndex++;
    int Point2 = TriIndex++;
    int Point3 = TriIndex++;

    Vertices.Add(A);
    Vertices.Add(B);
    Vertices.Add(C);

    Triangles.Add(Point1);
    Triangles.Add(Point2);
    Triangles.Add(Point3);

    FVector ThisNorm = FVector::CrossProduct(A - C, A - B).GetSafeNormal();
    for (int i = 0; i < 3; i++) {
        Normals.Add(ThisNorm);
        Tangents.Add(Tangent);
        VertexColors.Add(FColor::White);
    }

    UV0.Add(FVector2D(0.0f, 1.0f));
    UV0.Add(FVector2D(0.0f, 0.0f));
    UV0.Add(FVector2D(1.0f, 0.0f));
}

void AViewFrustum::AddQuadMesh(FVector A, FVector B, FVector C, FVector D, int32& TriIndex, FProcMeshTangent Tangent) {
    int Point1 = TriIndex++;
    int Point2 = TriIndex++;
    int Point3 = TriIndex++;
    int Point4 = TriIndex++;

    Vertices.Add(A);
    Vertices.Add(B);
    Vertices.Add(C);
    Vertices.Add(D);

    Triangles.Add(Point1);
    Triangles.Add(Point2);
    Triangles.Add(Point3);

    Triangles.Add(Point1);
    Triangles.Add(Point3);
    Triangles.Add(Point4);

    FVector ThisNorm = FVector::CrossProduct(A - C, A - B).GetSafeNormal();
    for (int i = 0; i < 4; i++) {
        Normals.Add(ThisNorm);
        Tangents.Add(Tangent);
        VertexColors.Add(FColor::White);
    }

    UV0.Add(FVector2D(0.0f, 0.0f));
    UV0.Add(FVector2D(0.0f, 1.0f));
    UV0.Add(FVector2D(1.0f, 1.0f));
    UV0.Add(FVector2D(1.0f, 0.0f));
}

void AViewFrustum::ClearMeshData() {
    Vertices.Reset();
    Triangles.Reset();
    Normals.Reset();
    Tangents.Reset();
    UV0.Reset();
    ProcMeshComp->ClearAllMeshSections();
}


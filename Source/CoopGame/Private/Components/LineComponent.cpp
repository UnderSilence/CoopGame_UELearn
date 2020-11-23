#include "Components/LineComponent.h"
#include "PrimitiveSceneProxy.h"
#include "SceneManagement.h"

const static FName NAME_LineCompResourceNameForDebugging(TEXT("LineComponent"));

class FLineSceneProxy;

struct FUpdateParams {
    FLineSceneProxy* SceneProxy;

    TArray<FVector> Points;

    float LineThickness;

    FLinearColor LineColor;

    bool bLoop = false;
};

class FLineSceneProxy : public FPrimitiveSceneProxy {
public:
    FLineSceneProxy(ULineComponent* InComponent)
        :FPrimitiveSceneProxy(InComponent, NAME_LineCompResourceNameForDebugging) {}

    virtual uint32 GetMemoryFootprint(void) const override { return (sizeof(*this) + GetAllocatedSize()); }

    virtual FPrimitiveViewRelevance GetViewRelevance(const FSceneView* View) const override {
        FPrimitiveViewRelevance Result;
        Result.bDrawRelevance = IsShown(View);
        Result.bDynamicRelevance = true;
        Result.bShadowRelevance = false;
        Result.bEditorPrimitiveRelevance = UseEditorCompositing(View);
        return Result;
    }

    virtual void GetDynamicMeshElements(const TArray<const FSceneView*>& Views, const FSceneViewFamily& ViewFamily, uint32 VisibilityMap, FMeshElementCollector& Collector) const override {
        QUICK_SCOPE_CYCLE_COUNTER(STAT_FLineSceneProxy_GetDynamicMeshElements);

        for (int32 ViewIndex = 0; ViewIndex < Views.Num(); ViewIndex++) {
            if (VisibilityMap & (1 << ViewIndex)) {
                const FSceneView* View = Views[ViewIndex];

                FPrimitiveDrawInterface* PDI = Collector.GetPDI(ViewIndex);

                if (UpdateParams.Points.Num() == 1)
                    PDI->DrawPoint(UpdateParams.Points[0], UpdateParams.LineColor, UpdateParams.LineThickness, 0);

                for (int i = 1; i < UpdateParams.Points.Num(); ++i) {
                    PDI->DrawLine(UpdateParams.Points[i - 1], UpdateParams.Points[i], UpdateParams.LineColor, 0, UpdateParams.LineThickness);
                }

                if (UpdateParams.bLoop) {
                    if (UpdateParams.Points.Num() > 2)
                        PDI->DrawLine(UpdateParams.Points[UpdateParams.Points.Num() - 1], UpdateParams.Points[0], UpdateParams.LineColor, 0, UpdateParams.LineThickness);
                }

            }
        }
    }

    void Update(const FUpdateParams& NewUpdateParams) {
        UpdateParams = NewUpdateParams;
    }


    virtual SIZE_T GetTypeHash() const override {
        static size_t UniquePointer;
        return reinterpret_cast<size_t>(&UniquePointer);
    }

private:
    FUpdateParams UpdateParams;
};

//////////////////////////////////////////////////////////////////////////
ULineComponent::ULineComponent() {
    LineColor = FLinearColor::White;
    LineThickness = 8.f;

    SetCollisionEnabled(ECollisionEnabled::NoCollision);
    SetCollisionResponseToAllChannels(ECR_Ignore);
}

ULineComponent::~ULineComponent() {
    Clear();
}

FPrimitiveSceneProxy* ULineComponent::CreateSceneProxy() {
    return new FLineSceneProxy(this);
}

FBoxSphereBounds ULineComponent::CalcBounds(const FTransform& LocalToWorld) const {
    return Bounds;
}

void ULineComponent::SetPoints(const TArray<FVector>& Points) {
    if (SceneProxy == nullptr) {
        //LOG_WARNING(TEXT("ULineComponent::SetPoints SceneProxy is null, your actor is hidden in game?"));
        return;
    }
    FUpdateParams UpdateParams;
    UpdateParams.SceneProxy = (FLineSceneProxy*)SceneProxy;
    UpdateParams.Points = Points;
    UpdateParams.LineThickness = LineThickness;
    UpdateParams.LineColor = LineColor;

    ENQUEUE_RENDER_COMMAND(ULineComponentUpdateCommand)(
        [UpdateParams](FRHICommandListImmediate& RHICmdList) {
            FScopeCycleCounter Context(UpdateParams.SceneProxy->GetStatId());
            UpdateParams.SceneProxy->Update(UpdateParams);
        });

    Bounds = FBoxSphereBounds(Points.GetData(), Points.Num());

    UpdateBounds();
    MarkRenderTransformDirty();
}

void ULineComponent::SetColor(const TArray<FVector>& Points, FLinearColor color, bool bLoop) {
    if (SceneProxy == nullptr) {
        //LOG_WARNING(TEXT("ULineComponent::SetPoints SceneProxy is null, your actor is hidden in game?"));
        return;
    }
    FUpdateParams UpdateParams;
    UpdateParams.SceneProxy = (FLineSceneProxy*)SceneProxy;
    UpdateParams.Points = Points;
    UpdateParams.LineThickness = LineThickness;
    UpdateParams.LineColor = color;
    UpdateParams.bLoop = bLoop;

    ENQUEUE_RENDER_COMMAND(ULineComponentUpdateCommand)(
        [UpdateParams](FRHICommandListImmediate& RHICmdList) {
            FScopeCycleCounter Context(UpdateParams.SceneProxy->GetStatId());
            UpdateParams.SceneProxy->Update(UpdateParams);
        });

    Bounds = FBoxSphereBounds(Points.GetData(), Points.Num());

    UpdateBounds();
    MarkRenderTransformDirty();
}

void ULineComponent::Clear() {
    SetPoints(TArray<FVector>());
}

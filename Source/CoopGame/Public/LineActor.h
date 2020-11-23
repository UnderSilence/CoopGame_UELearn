#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LineActor.generated.h"

class ULineComponent;

/**
	Used for rendering path
*/
UCLASS()
class COOPGAME_API  ALineActor : public AActor {
	GENERATED_BODY()

public:

	ALineActor();
	virtual ~ALineActor();

	UFUNCTION(BlueprintCallable, Category = "PathGuide")
	virtual void SetCustomPoints(const TArray<FVector>& Points);

	UFUNCTION(BlueprintCallable, Category = "PathGuide")
	virtual void Clear();

	FLinearColor PathColor;

	float PathThickness;

	//float ZOffset;
public:
	void SetColor(FLinearColor color, bool bLoop);

protected:

	virtual void BeginPlay() override;

	TArray<FVector> m_Points;

	UPROPERTY()
	ULineComponent* PathLineComponent;
};

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NexoStationRun.generated.h"

class SWidget;
class UPrimitiveComponent;
class USceneComponent;
class UTextRenderComponent;

UCLASS()
class MYPROJECT3_API ANexoStationRun : public AActor
{
    GENERATED_BODY()

public:
    ANexoStationRun();
    virtual void Tick(float DeltaSeconds) override;

    UFUNCTION(BlueprintCallable, Category = "Station")
    bool TryBoardTrain();

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
    UPROPERTY(VisibleAnywhere, Category = "Station")
    TObjectPtr<USceneComponent> Train;

    struct FDoorLeaf
    {
        USceneComponent* Component;
        FVector ClosedPosition;
        float SlideDistance;
    };

    TArray<FDoorLeaf> DoorLeaves;
    TArray<UPrimitiveComponent*> ClosedDoorBlockers;
    TArray<UPrimitiveComponent*> BoardingSteps;
    TArray<UTextRenderComponent*> BoardingSigns;

    TSharedPtr<SWidget> CompletionScreen;
    double TrainStartTime = -1.0;
    float DoorOpenness = 0.0f;
    bool bFinished = false;

    void FinishRun();
};

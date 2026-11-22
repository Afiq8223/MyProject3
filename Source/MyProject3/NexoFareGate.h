#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NexoFareGate.generated.h"

class UPhysicsConstraintComponent;
class UStaticMeshComponent;
class UMaterialInterface;

UCLASS()
class MYPROJECT3_API ANexoFareGate : public AActor
{
    GENERATED_BODY()

public:
    ANexoFareGate();
    virtual void Tick(float DeltaSeconds) override;

protected:
    virtual void BeginPlay() override;

private:
    UPROPERTY(EditInstanceOnly, Category = "Fare Gate")
    TObjectPtr<AActor> StatusIndicator;

    UPROPERTY()
    TObjectPtr<UMaterialInterface> LockedMaterial;

    UPROPERTY()
    TObjectPtr<UMaterialInterface> ReadyMaterial;

    UPROPERTY(VisibleAnywhere, Category = "Fare Gate")
    TObjectPtr<UStaticMeshComponent> Flap;

    UPROPERTY(VisibleAnywhere, Category = "Fare Gate")
    TObjectPtr<UPhysicsConstraintComponent> Hinge;

    float OpenAngle = 0.0f;
    bool bUnlocked = false;
};

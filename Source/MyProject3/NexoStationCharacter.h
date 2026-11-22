#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "NexoStationCharacter.generated.h"

class UAnimSequence;
class UCameraComponent;

UCLASS()
class MYPROJECT3_API ANexoStationCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    ANexoStationCharacter();
    virtual void Tick(float DeltaSeconds) override;

    void GrantStationTicket() { bHasStationTicket = true; }
    void RecordFareGateCrossing() { bCrossedFareGate = true; }
    bool HasCrossedFareGate() const { return bCrossedFareGate; }

    UFUNCTION(BlueprintPure, Category = "Station")
    bool HasStationTicket() const { return bHasStationTicket; }

protected:
    virtual void BeginPlay() override;

private:
    bool bHasStationTicket = false;
    bool bCrossedFareGate = false;
    UPROPERTY(VisibleAnywhere, Category = "First Person")
    TObjectPtr<UCameraComponent> FirstPersonCamera;

    UPROPERTY(EditDefaultsOnly, Category = "Body Appearance")
    TObjectPtr<UAnimSequence> IdleAnimation;

    UPROPERTY(EditDefaultsOnly, Category = "Body Appearance")
    TObjectPtr<UAnimSequence> WalkAnimation;

    UPROPERTY(EditDefaultsOnly, Category = "Body Appearance")
    FName HiddenHeadBone = TEXT("head");

    UPROPERTY(Transient)
    TObjectPtr<UAnimSequence> CurrentAnimation;
};

#pragma once

#include "CoreMinimal.h"
#include "Fonts/SlateFontInfo.h"
#include "GameFramework/Actor.h"
#include "NexoKioskLanguage.h"
#include "NexoStationService.generated.h"

class USceneComponent;
class USkeletalMesh;
class USkeletalMeshComponent;
class UStaticMeshComponent;
class UTextRenderComponent;
class UUserWidget;
class UWidgetSwitcher;
class UAnimSequence;
class UMaterialInterface;
class UPointLightComponent;
class UStaticMesh;
class UTextBlock;
class ANexoStationService;

UCLASS()
class UNexoKioskBinding : public UObject
{
    GENERATED_BODY()

    friend class ANexoStationService;
    TWeakObjectPtr<ANexoStationService> Service;
    TWeakObjectPtr<AActor> Kiosk;
    TWeakObjectPtr<UUserWidget> Widget;
    TWeakObjectPtr<UWidgetSwitcher> Pages;
    TWeakObjectPtr<UStaticMeshComponent> BeaconLens;
    TWeakObjectPtr<UPointLightComponent> BeaconLight;
    int32 LastPage = INDEX_NONE;
    int32 PageBeforeLanguageChange = INDEX_NONE;
    double VoiceBeaconUntil = 0.0;
    ENexoKioskLanguage Language = ENexoKioskLanguage::English;

    struct FLabel
    {
        TWeakObjectPtr<UTextBlock> Widget;
        FSlateFontInfo OriginalFont;
        FString English;
        FString Displayed;
    };
    TArray<FLabel> Labels;
    void RefreshLabels(bool bUpdateFont);

    UFUNCTION()
    void OnAssistanceClicked();

    UFUNCTION()
    void OnVoiceAssistanceClicked();

    UFUNCTION()
    void OnLanguageClicked();

};

UCLASS()
class MYPROJECT3_API ANexoStationService : public AActor
{
    GENERATED_BODY()

public:
    ANexoStationService();
    virtual void OnConstruction(const FTransform& Transform) override;
    virtual void Tick(float DeltaSeconds) override;

protected:
    virtual void BeginPlay() override;

private:
    friend class UNexoKioskBinding;
    enum class EAttendantState : uint8 { AtOffice, ToKiosk, AtKiosk, Returning };

    UPROPERTY(EditInstanceOnly, Category = "Station")
    TObjectPtr<AActor> Kiosk;

    UPROPERTY(VisibleAnywhere, Category = "Station")
    TObjectPtr<USceneComponent> DoorHinge;

    UPROPERTY(VisibleAnywhere, Category = "Station")
    TObjectPtr<UStaticMeshComponent> Door;

    UPROPERTY(VisibleAnywhere, Category = "Station")
    TObjectPtr<USkeletalMeshComponent> Attendant;

    UPROPERTY(VisibleAnywhere, Category = "Station")
    TObjectPtr<UTextRenderComponent> Speech;

    UPROPERTY(EditAnywhere, Category = "Staff Appearance")
    TObjectPtr<USkeletalMesh> AttendantMesh;

    UPROPERTY(EditAnywhere, Category = "Staff Appearance")
    TObjectPtr<UAnimSequence> IdleAnimation;

    UPROPERTY(EditAnywhere, Category = "Staff Appearance")
    TObjectPtr<UAnimSequence> WalkAnimation;

    UPROPERTY()
    TObjectPtr<UStaticMesh> BeaconMesh;

    UPROPERTY()
    TObjectPtr<UMaterialInterface> DisplayMaterial;

    UPROPERTY()
    TObjectPtr<UMaterialInterface> BeaconMaterial;

    UPROPERTY()
    TObjectPtr<UMaterialInterface> BeaconHousingMaterial;

    UPROPERTY(Transient)
    TArray<TObjectPtr<UNexoKioskBinding>> KioskBindings;

    TWeakObjectPtr<AActor> HelpKiosk;
    TWeakObjectPtr<AActor> PendingKiosk;
    EAttendantState State = EAttendantState::AtOffice;
    int32 Waypoint = 0;
    float DoorYaw = 0.0f;
    float DoorHold = 0.0f;
    float SpeechHold = 0.0f;
    float VisitTime = 0.0f;
    FVector KioskApproach = FVector::ZeroVector;
    FVector KioskDestination = FVector::ZeroVector;

    void UpdateKiosk(UNexoKioskBinding& Binding);
    void InstallBeacon(AActor& StationKiosk, UNexoKioskBinding& Binding);
    void UpdateAttendant(float DeltaSeconds);
    void Speak(const FText& Message);
    void SetWalking(bool bWalking);

    void RequestAssistance(AActor* RequestedKiosk);
};

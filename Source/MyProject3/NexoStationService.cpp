#include "NexoStationService.h"
#include "NexoStationCharacter.h"

#include "Animation/AnimSequence.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Components/BoxComponent.h"
#include "Components/Button.h"
#include "Components/PointLightComponent.h"
#include "Components/SceneComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TextBlock.h"
#include "Components/TextRenderComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/WidgetSwitcher.h"
#include "Engine/SkeletalMesh.h"
#include "Engine/StaticMesh.h"
#include "Fonts/CompositeFont.h"
#include "GameFramework/PlayerController.h"
#include "InputCoreTypes.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInterface.h"
#include "Misc/Paths.h"
#include "UObject/ConstructorHelpers.h"
#include "UObject/UnrealType.h"

#define LOCTEXT_NAMESPACE "NexoStationService"

namespace
{
    const FVector AttendantRoute[] = {
        FVector(105, -143, 0),
        FVector(90, 0, 0),
        FVector(-98, 0, 0),
        FVector(-98, 490, 0),
    };
    constexpr int32 KioskStop = UE_ARRAY_COUNT(AttendantRoute) + 1;
    const FName AssistanceButton(TEXT("BTN_HelpPerson"));
    const FName VoiceAssistanceButton(TEXT("BTN_HelpVoice"));
    const FName LanguageButton(TEXT("BTN_HdrLang"));
    const FName TextSizeButton(TEXT("BTN_HdrTextSize"));

    FSlateFontInfo MakeKioskFont()
    {
        const FString Fonts = FPaths::ProjectContentDir() / TEXT("Station/Fonts");
        TSharedRef<FCompositeFont> Font = MakeShared<FCompositeFont>(TEXT("Regular"),
            Fonts / TEXT("NotoSansSC.ttf"), EFontHinting::Default, EFontLoadingPolicy::LazyLoad);
        FCompositeSubFont& Tamil = Font->SubTypefaces.AddDefaulted_GetRef();
        Tamil.CharacterRanges.Add(FInt32Range::Inclusive(0x0B80, 0x0BFF));
        Tamil.Typeface.AppendFont(TEXT("Regular"), Fonts / TEXT("NotoSansTamil.ttf"),
            EFontHinting::Default, EFontLoadingPolicy::LazyLoad);
        return FSlateFontInfo(Font, 18.0f);
    }
}

ANexoStationService::ANexoStationService()
{
    PrimaryActorTick.bCanEverTick = true;
    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("OfficeEntrance"));

    DoorHinge = CreateDefaultSubobject<USceneComponent>(TEXT("OfficeDoorHinge"));
    DoorHinge->SetupAttachment(RootComponent);
    DoorHinge->SetRelativeLocation(FVector(0, -73.5, 0));

    Door = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("OfficeDoor"));
    Door->SetupAttachment(DoorHinge);
    Door->SetRelativeLocation(FVector(0, 73.5, 124.5));
    Door->SetRelativeScale3D(FVector(0.06, 1.47, 2.49));
    Door->SetCollisionProfileName(TEXT("BlockAllDynamic"));

    UBoxComponent* StaffOnlyBarrier = CreateDefaultSubobject<UBoxComponent>(TEXT("StaffOnlyBarrier"));
    StaffOnlyBarrier->SetupAttachment(RootComponent);
    StaffOnlyBarrier->SetRelativeLocation(FVector(0, 0, 112));
    StaffOnlyBarrier->InitBoxExtent(FVector(7, 73, 112));
    StaffOnlyBarrier->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    StaffOnlyBarrier->SetCollisionResponseToAllChannels(ECR_Ignore);
    StaffOnlyBarrier->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);

    const ConstructorHelpers::FObjectFinder<UStaticMesh> Cube(TEXT("/Engine/BasicShapes/Cube.Cube"));
    BeaconMesh = Cube.Object;
    Door->SetStaticMesh(Cube.Object);
    const ConstructorHelpers::FObjectFinder<UMaterialInterface> Teal(TEXT("/Game/Station/M_MRT_Teal.M_MRT_Teal"));
    Door->SetMaterial(0, Teal.Object);
    const ConstructorHelpers::FObjectFinder<UMaterialInterface> Display(TEXT("/Game/Station/M_MRT_KioskDisplay.M_MRT_KioskDisplay"));
    DisplayMaterial = Display.Object;
    const ConstructorHelpers::FObjectFinder<UMaterialInterface> Beacon(TEXT("/Game/Station/M_MRT_AssistanceRed.M_MRT_AssistanceRed"));
    BeaconMaterial = Beacon.Object;
    const ConstructorHelpers::FObjectFinder<UMaterialInterface> Housing(TEXT("/Game/Station/M_MRT_Charcoal.M_MRT_Charcoal"));
    BeaconHousingMaterial = Housing.Object;

    Attendant = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("StationAttendant"));
    Attendant->SetupAttachment(RootComponent);
    Attendant->SetRelativeLocationAndRotation(AttendantRoute[0], FRotator(0, 90, 0));
    Attendant->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    const ConstructorHelpers::FObjectFinder<USkeletalMesh> Body(
        TEXT("/Game/Characters/Mannequins/Meshes/SKM_Manny_Simple.SKM_Manny_Simple"));
    AttendantMesh = Body.Object;
    Attendant->SetSkeletalMeshAsset(AttendantMesh);

    const ConstructorHelpers::FObjectFinder<UAnimSequence> Idle(
        TEXT("/Game/Characters/Mannequins/Anims/Unarmed/MM_Idle.MM_Idle"));
    IdleAnimation = Idle.Object;
    const ConstructorHelpers::FObjectFinder<UAnimSequence> Walk(
        TEXT("/Game/Characters/Mannequins/Anims/Unarmed/Walk/MF_Unarmed_Walk_Fwd.MF_Unarmed_Walk_Fwd"));
    WalkAnimation = Walk.Object;

    Speech = CreateDefaultSubobject<UTextRenderComponent>(TEXT("AttendantDialogue"));
    Speech->SetupAttachment(Attendant);
    Speech->SetRelativeLocation(FVector(0, 0, 205));
    Speech->SetWorldSize(9.0f);
    Speech->SetHorizontalAlignment(EHTA_Center);
    Speech->SetTextRenderColor(FColor(255, 249, 222));
    Speech->SetVisibility(false);
}

void ANexoStationService::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);
    Attendant->SetSkeletalMeshAsset(AttendantMesh);
}

void ANexoStationService::BeginPlay()
{
    Super::BeginPlay();
    if (!Kiosk)
    {
        return;
    }
    TArray<AActor*> StationKiosks;
    UGameplayStatics::GetAllActorsOfClass(this, Kiosk->GetClass(), StationKiosks);
    for (AActor* StationKiosk : StationKiosks)
    {
        UNexoKioskBinding* Binding = NewObject<UNexoKioskBinding>(this);
        Binding->Service = this;
        Binding->Kiosk = StationKiosk;
        KioskBindings.Add(Binding);
        if (UWidgetComponent* Screen = StationKiosk->FindComponentByClass<UWidgetComponent>(); Screen && DisplayMaterial)
        {
            Screen->SetMaterial(0, DisplayMaterial);
        }
        InstallBeacon(*StationKiosk, *Binding);
    }
}

void ANexoStationService::InstallBeacon(AActor& StationKiosk, UNexoKioskBinding& Binding)
{
    const auto Mount = [&StationKiosk](USceneComponent& Part, const FVector& Offset)
    {
        Part.SetupAttachment(StationKiosk.GetRootComponent());
        Part.SetRelativeLocation(Offset);
        StationKiosk.AddInstanceComponent(&Part);
        Part.RegisterComponent();
    };
    UStaticMeshComponent* Housing = NewObject<UStaticMeshComponent>(&StationKiosk, TEXT("AssistanceBeaconHousing"));
    Housing->SetStaticMesh(BeaconMesh);
    Housing->SetMaterial(0, BeaconHousingMaterial);
    Housing->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    Housing->SetRelativeScale3D(FVector(0.15, 0.15, 0.08));
    Mount(*Housing, FVector(0, 0, 184));

    UStaticMeshComponent* Lens = NewObject<UStaticMeshComponent>(&StationKiosk, TEXT("AssistanceBeaconLens"));
    Lens->SetStaticMesh(BeaconMesh);
    Lens->SetMaterial(0, BeaconMaterial);
    Lens->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    Lens->SetCastShadow(false);
    Lens->SetRelativeScale3D(FVector(0.13, 0.13, 0.075));
    Lens->SetVisibility(false);
    Mount(*Lens, FVector(0, 0, 191));
    Binding.BeaconLens = Lens;

    UPointLightComponent* Light = NewObject<UPointLightComponent>(&StationKiosk, TEXT("AssistanceBeaconLight"));
    Light->SetLightColor(FLinearColor(1.0f, 0.012f, 0.006f));
    Light->SetIntensity(165.0f);
    Light->SetAttenuationRadius(145.0f);
    Light->SetCastShadows(false);
    Light->SetVisibility(false);
    Mount(*Light, FVector(0, 0, 199));
    Binding.BeaconLight = Light;
}

void UNexoKioskBinding::OnAssistanceClicked()
{
    if (ANexoStationService* StationService = Service.Get())
    {
        StationService->RequestAssistance(Kiosk.Get());
    }
}

void UNexoKioskBinding::OnLanguageClicked()
{
    PageBeforeLanguageChange = LastPage;
    Language = static_cast<ENexoKioskLanguage>((static_cast<uint8>(Language) + 1) % 4);
    RefreshLabels(true);
}

void UNexoKioskBinding::OnVoiceAssistanceClicked()
{
    if (const ANexoStationService* StationService = Service.Get())
    {
        VoiceBeaconUntil = StationService->GetWorld()->GetTimeSeconds() + 6.0;
    }
}

void UNexoKioskBinding::RefreshLabels(bool bUpdateFont)
{
    static const FSlateFontInfo MultilingualFont = MakeKioskFont();
    for (FLabel& Label : Labels)
    {
        UTextBlock* Text = Label.Widget.Get();
        if (!Text)
        {
            continue;
        }
        const FString Current = Text->GetText().ToString();
        const bool bChanged = Current != Label.Displayed;
        if (bChanged)
        {
            Label.English = Current;
        }
        const bool bMultilingualFont = Language == ENexoKioskLanguage::Tamil || Language == ENexoKioskLanguage::Chinese;
        if (bUpdateFont && bMultilingualFont)
        {
            FSlateFontInfo Font = MultilingualFont;
            const float LanguageScale = Language == ENexoKioskLanguage::Tamil ? 0.86f : 1.0f;
            Font.Size = Label.OriginalFont.Size * LanguageScale;
            Text->SetFont(Font);
        }
        if (bChanged || bUpdateFont)
        {
            Label.Displayed = TranslateKioskText(Text->GetFName(), Label.English, Language);
            if (Label.Displayed != Current)
            {
                Text->SetText(FText::FromString(Label.Displayed));
            }
        }
        if (bUpdateFont && !bMultilingualFont)
        {
            Text->SetFont(Label.OriginalFont);
        }
    }
}

void ANexoStationService::UpdateKiosk(UNexoKioskBinding& Binding)
{
    const AActor* StationKiosk = Binding.Kiosk.Get();
    const UWidgetComponent* Screen = StationKiosk ? StationKiosk->FindComponentByClass<UWidgetComponent>() : nullptr;
    UUserWidget* Widget = Screen ? Screen->GetUserWidgetObject() : nullptr;
    if (Binding.Widget.Get() != Widget)
    {
        if (UUserWidget* OldWidget = Binding.Widget.Get())
        {
            if (UButton* Button = Cast<UButton>(OldWidget->GetWidgetFromName(AssistanceButton)))
            {
                Button->OnClicked.RemoveDynamic(&Binding, &UNexoKioskBinding::OnAssistanceClicked);
            }
            if (UButton* Button = Cast<UButton>(OldWidget->GetWidgetFromName(LanguageButton)))
            {
                Button->OnClicked.RemoveDynamic(&Binding, &UNexoKioskBinding::OnLanguageClicked);
            }
            if (UButton* Button = Cast<UButton>(OldWidget->GetWidgetFromName(VoiceAssistanceButton)))
            {
                Button->OnClicked.RemoveDynamic(&Binding, &UNexoKioskBinding::OnVoiceAssistanceClicked);
            }
        }
        Binding.Widget = Widget;
        Binding.Pages = Widget ? Cast<UWidgetSwitcher>(Widget->GetWidgetFromName(TEXT("WS_Pages"))) : nullptr;
        Binding.LastPage = INDEX_NONE;
        if (UButton* Button = Widget ? Cast<UButton>(Widget->GetWidgetFromName(AssistanceButton)) : nullptr)
        {
            Button->OnClicked.AddUniqueDynamic(&Binding, &UNexoKioskBinding::OnAssistanceClicked);
        }
        if (UButton* Button = Widget ? Cast<UButton>(Widget->GetWidgetFromName(LanguageButton)) : nullptr)
        {
            Button->OnClicked.Clear();
            Button->OnClicked.AddUniqueDynamic(&Binding, &UNexoKioskBinding::OnLanguageClicked);
        }
        if (UButton* Button = Widget ? Cast<UButton>(Widget->GetWidgetFromName(TextSizeButton)) : nullptr)
        {
            Button->OnClicked.Clear();
            Button->SetVisibility(ESlateVisibility::Collapsed);
            Button->RemoveFromParent();
        }
        if (UButton* Button = Widget ? Cast<UButton>(Widget->GetWidgetFromName(VoiceAssistanceButton)) : nullptr)
        {
            Button->OnClicked.AddUniqueDynamic(&Binding, &UNexoKioskBinding::OnVoiceAssistanceClicked);
        }
        Binding.Labels.Reset();
        if (Widget && Widget->WidgetTree)
        {
            Widget->WidgetTree->ForEachWidget([&Binding](UWidget* Child)
            {
                if (UTextBlock* Text = Cast<UTextBlock>(Child))
                {
                    const FString English = Text->GetText().ToString();
                    Binding.Labels.Add({Text, Text->GetFont(), English, English});
                }
            });
            Binding.RefreshLabels(true);
        }
    }

    if (Binding.Language != ENexoKioskLanguage::English)
    {
        Binding.RefreshLabels(false);
    }
    const double Now = GetWorld()->GetTimeSeconds();
    const bool bWaiting = StationKiosk && (Now < Binding.VoiceBeaconUntil || PendingKiosk.Get() == StationKiosk
        || (HelpKiosk.Get() == StationKiosk && State == EAttendantState::ToKiosk));
    const bool bBlinkOn = bWaiting && FMath::Frac(Now * 1.65) < 0.5;
    if (UStaticMeshComponent* Lens = Binding.BeaconLens.Get())
    {
        Lens->SetVisibility(bBlinkOn);
    }
    if (UPointLightComponent* Light = Binding.BeaconLight.Get())
    {
        Light->SetVisibility(bBlinkOn);
    }

    UWidgetSwitcher* Switcher = Binding.Pages.Get();
    if (!Switcher || !Widget)
    {
        return;
    }
    if (Binding.PageBeforeLanguageChange != INDEX_NONE)
    {
        const int32 PreviousPage = Binding.PageBeforeLanguageChange;
        Binding.PageBeforeLanguageChange = INDEX_NONE;
        const UTextBlock* NoticeTitle = Cast<UTextBlock>(Widget->GetWidgetFromName(TEXT("TXT_RecTitle")));
        const UWidget* CurrentPage = Switcher->GetActiveWidget();
        if (CurrentPage && CurrentPage->GetFName() == FName(TEXT("P10_Recovery")) && NoticeTitle
            && NoticeTitle->GetText().ToString().Contains(TEXT("Accessibility & Language")))
        {
            // A retained Blueprint accessibility handler may have opened its shared notice page.
            if (UFunction* Back = Widget->FindFunction(TEXT("OnBackFromOverlay")); Back && Back->ParmsSize == 0)
            {
                Widget->ProcessEvent(Back, nullptr);
            }
            if (Switcher->GetActiveWidgetIndex() != PreviousPage)
            {
                Switcher->SetActiveWidgetIndex(PreviousPage);
            }
        }
    }
    const int32 ActivePage = Switcher->GetActiveWidgetIndex();
    const UWidget* Page = Switcher->GetActiveWidget();
    if (ActivePage != Binding.LastPage && Page && Page->GetFName() == FName(TEXT("P7_Done")))
    {
        const FBoolProperty* TopUpFlow = FindFProperty<FBoolProperty>(Widget->GetClass(), TEXT("bIsTopUpFlow"));
        if (TopUpFlow && !TopUpFlow->GetPropertyValue_InContainer(Widget))
        {
            if (ANexoStationCharacter* Player = Cast<ANexoStationCharacter>(UGameplayStatics::GetPlayerPawn(this, 0)))
            {
                Player->GrantStationTicket();
            }
        }
    }
    Binding.LastPage = ActivePage;
}

void ANexoStationService::RequestAssistance(AActor* RequestedKiosk)
{
    if (!RequestedKiosk)
    {
        return;
    }
    if (State == EAttendantState::AtKiosk && HelpKiosk.Get() == RequestedKiosk)
    {
        VisitTime = 14.0f;
        Speak(LOCTEXT("AtKiosk", "I can help with that.\nPick a destination, then a ticket type."));
    }
    else if (State == EAttendantState::AtOffice)
    {
        HelpKiosk = RequestedKiosk;
        const UWidgetComponent* Screen = RequestedKiosk->FindComponentByClass<UWidgetComponent>();
        const FVector Facing = Screen ? Screen->GetForwardVector().GetSafeNormal2D() : -RequestedKiosk->GetActorRightVector();
        const FVector Beside = FVector::CrossProduct(FVector::UpVector, Facing);
        const FVector Destination = RequestedKiosk->GetActorLocation() + Facing * 125.0 + Beside * 110.0;
        KioskDestination = GetActorTransform().InverseTransformPosition(Destination);
        KioskDestination.Z = 0.0;
        KioskApproach = KioskDestination;
        KioskApproach.Y -= 42.0;
        State = EAttendantState::ToKiosk;
        Waypoint = 1;
        SetWalking(true);
    }
    else if (HelpKiosk.Get() != RequestedKiosk || State == EAttendantState::Returning)
    {
        PendingKiosk = RequestedKiosk;
    }
}

void ANexoStationService::SetWalking(bool bWalking)
{
    Attendant->PlayAnimation(bWalking ? WalkAnimation : IdleAnimation, true);
    Attendant->SetPlayRate(bWalking ? 0.82f : 1.0f);
}

void ANexoStationService::Speak(const FText& Message)
{
    SpeechHold = 8.0f;
    Speech->SetText(Message);
    Speech->SetVisibility(true);
}

void ANexoStationService::UpdateAttendant(float DeltaSeconds)
{
    if (State == EAttendantState::AtKiosk)
    {
        VisitTime -= DeltaSeconds;
        if (VisitTime <= 0.0f)
        {
            State = EAttendantState::Returning;
            Waypoint = KioskStop - 1;
            SetWalking(true);
        }
        return;
    }
    if (State == EAttendantState::AtOffice)
    {
        if (!Attendant->GetSingleNodeInstance())
        {
            SetWalking(false);
        }
        return;
    }

    const FVector Position = Attendant->GetRelativeLocation();
    const FVector Goal = Waypoint == KioskStop ? KioskDestination
        : Waypoint == KioskStop - 1 ? KioskApproach : AttendantRoute[Waypoint];
    const FVector Direction = Goal - Position;
    // Open the door before reaching the jamb, including on the return trip.
    if (Waypoint <= 3 && FMath::Abs(Position.Y) < 160.0 && FMath::Abs(Position.X) < 160.0)
    {
        DoorHold = 1.6f;
    }
    if (Direction.SizeSquared() > 1.0)
    {
        FRotator Facing = Direction.Rotation();
        Facing.Yaw -= 90.0f;
        Attendant->SetRelativeRotation(FMath::RInterpTo(Attendant->GetRelativeRotation(), Facing, DeltaSeconds, 8.0f));
        Attendant->SetRelativeLocation(FMath::VInterpConstantTo(Position, Goal, DeltaSeconds, 153.0f));
        return;
    }
    if (State == EAttendantState::ToKiosk && Waypoint == KioskStop)
    {
        State = EAttendantState::AtKiosk;
        VisitTime = 14.0f;
        SetWalking(false);
        if (const AActor* ActiveKiosk = HelpKiosk.Get())
        {
            const FVector ToKiosk = ActiveKiosk->GetActorLocation() - Attendant->GetComponentLocation();
            Attendant->SetWorldRotation(FRotator(0, ToKiosk.Rotation().Yaw - 90.0, 0));
        }
        Speak(LOCTEXT("Arrival", "Hi, I'm with station staff.\nNeed help buying a ticket? Press E."));
    }
    else if (State == EAttendantState::Returning && Waypoint == 0)
    {
        State = EAttendantState::AtOffice;
        Attendant->SetRelativeRotation(FRotator(0, 90, 0));
        SetWalking(false);
        HelpKiosk.Reset();
        if (AActor* NextKiosk = PendingKiosk.Get())
        {
            PendingKiosk.Reset();
            RequestAssistance(NextKiosk);
        }
    }
    else
    {
        Waypoint += State == EAttendantState::ToKiosk ? 1 : -1;
    }
}

void ANexoStationService::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    for (UNexoKioskBinding* Binding : KioskBindings)
    {
        UpdateKiosk(*Binding);
    }
    UpdateAttendant(DeltaSeconds);
    DoorHold -= DeltaSeconds;
    SpeechHold -= DeltaSeconds;

    const APlayerController* Controller = UGameplayStatics::GetPlayerController(this, 0);
    const APawn* Player = Controller ? Controller->GetPawn() : nullptr;
    if (Player)
    {
        const FVector Local = GetActorTransform().InverseTransformPosition(Player->GetActorLocation());
        if (Controller->WasInputKeyJustPressed(EKeys::E) && FMath::Abs(Local.Z - 90.0) < 100.0)
        {
            if (FMath::Abs(Local.X) < 135.0 && FMath::Abs(Local.Y) < 72.0)
            {
                if (State == EAttendantState::AtOffice)
                {
                    Speak(LOCTEXT("Door", "Staff only. Please use the service window."));
                }
            }
            else if ((State == EAttendantState::AtOffice && FMath::Abs(Local.X) < 130.0 &&
                FMath::Abs(Local.Y + 146.0) < 72.0) ||
                (State == EAttendantState::AtKiosk &&
                    FVector::DistSquared2D(Player->GetActorLocation(), Attendant->GetComponentLocation()) < FMath::Square(225.0)))
            {
                Speak(LOCTEXT("Help", "Buy a ticket at the kiosk.\nThen follow the green gates to Platform 1."));
            }
        }
        if (SpeechHold > 0.0f)
        {
            Speech->SetWorldRotation((Player->GetPawnViewLocation() - Speech->GetComponentLocation()).Rotation());
        }
    }
    Speech->SetVisibility(SpeechHold > 0.0f);

    const float Target = DoorHold > 0.0f ? -95.0f : 0.0f;
    DoorYaw = FMath::FInterpConstantTo(DoorYaw, Target, DeltaSeconds, 145.0f);
    DoorHinge->SetRelativeRotation(FRotator(0, DoorYaw, 0));
}

#undef LOCTEXT_NAMESPACE


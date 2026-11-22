#include "NexoStationRun.h"
#include "NexoStationCharacter.h"

#include "CollisionQueryParams.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "Engine/GameViewportClient.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "InputCoreTypes.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInterface.h"
#include "Styling/CoreStyle.h"
#include "UObject/ConstructorHelpers.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Text/STextBlock.h"

#define LOCTEXT_NAMESPACE "NexoStationRun"

namespace
{
    constexpr float StartingY = -4050.0f;
    constexpr float ArrivalSeconds = 4.6f;
    constexpr float DoorOpeningSeconds = 1.05f;
    constexpr float TrainDoorX = 145.0f;
    constexpr float ScreenDoorX = 181.0f;
    constexpr float DoorHalfWidth = 80.0f;
    constexpr float DoorWidth = DoorHalfWidth * 2.0f;
    constexpr float BoardingReach = 435.0f;
    const FName SlidePositive(TEXT("SlidePositive"));
    const FName SlideNegative(TEXT("SlideNegative"));
    const FName ClosedDoorBlocker(TEXT("ClosedDoorBlocker"));
    const FName BoardingStep(TEXT("BoardingStep"));

    float DoorCenter(int32 Car, int32 Bay)
    {
        return (Car - 1) * 790.0f + Bay * 251.0f;
    }
}

ANexoStationRun::ANexoStationRun()
{
    PrimaryActorTick.bCanEverTick = true;
    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("StationRun"));
    Train = CreateDefaultSubobject<USceneComponent>(TEXT("PlatformTrain"));
    Train->SetupAttachment(RootComponent);
    Train->SetRelativeLocation(FVector(0, StartingY, 0));

    const ConstructorHelpers::FObjectFinder<UStaticMesh> Cube(TEXT("/Engine/BasicShapes/Cube.Cube"));
    const ConstructorHelpers::FObjectFinder<UMaterialInterface> Metal(TEXT("/Game/Station/M_MRT_WallPanel.M_MRT_WallPanel"));
    const ConstructorHelpers::FObjectFinder<UMaterialInterface> Dark(TEXT("/Game/Station/M_MRT_Charcoal.M_MRT_Charcoal"));
    const ConstructorHelpers::FObjectFinder<UMaterialInterface> Silver(TEXT("/Game/Station/M_MRT_Metal.M_MRT_Metal"));
    const ConstructorHelpers::FObjectFinder<UMaterialInterface> Red(TEXT("/Game/Station/M_MRT_NorthSouthRed.M_MRT_NorthSouthRed"));
    const ConstructorHelpers::FObjectFinder<UMaterialInterface> Lamps(TEXT("/Game/Station/M_MRT_Diffuser.M_MRT_Diffuser"));

    const auto AddPart = [this, &Cube](USceneComponent* Parent, const FString& Name, const FVector& Location, const FVector& Dimensions, UMaterialInterface* Material, bool bSolid = false)
    {
        UStaticMeshComponent* Part = CreateDefaultSubobject<UStaticMeshComponent>(*Name);
        Part->SetupAttachment(Parent);
        Part->SetStaticMesh(Cube.Object);
        Part->SetMaterial(0, Material);
        Part->SetRelativeLocation(Location);
        Part->SetRelativeScale3D(Dimensions / 100.0);
        Part->SetCollisionProfileName(bSolid ? TEXT("BlockAll") : TEXT("NoCollision"));
        return Part;
    };

    const auto TrainPart = [&AddPart, this](const FString& Name, const FVector& Location, const FVector& Dimensions, UMaterialInterface* Material, bool bSolid = false)
    {
        AddPart(Train, Name, Location, Dimensions, Material, bSolid);
    };
    const auto AddLeaf = [this](USceneComponent* Parent, const FString& Name, float X, float Y, int32 Side)
    {
        USceneComponent* Leaf = CreateDefaultSubobject<USceneComponent>(*Name);
        Leaf->SetupAttachment(Parent);
        Leaf->SetRelativeLocation(FVector(X + Side * 2.5f, Y + Side * DoorHalfWidth * 0.5f, 0));
        Leaf->ComponentTags.Add(Side > 0 ? SlidePositive : SlideNegative);
        return Leaf;
    };
    float ScreenInfillStart = -1470.0f;

    for (int32 Index = 0; Index < 3; ++Index)
    {
        const float Y = DoorCenter(Index, 0);
        const FString Prefix = FString::Printf(TEXT("Car%d_"), Index + 1);
        TrainPart(Prefix + TEXT("FarWall"), FVector(-129, Y, 115), FVector(12, 776, 224), Metal.Object, true);
        TrainPart(Prefix + TEXT("Floor"), FVector(0, Y, 2), FVector(270, 790, 16), Silver.Object, true);
        TrainPart(Prefix + TEXT("Roof"), FVector(0, Y, 251), FVector(270, 776, 27), Silver.Object, true);
        TrainPart(Prefix + TEXT("Undercarriage"), FVector(0, Y, -19), FVector(203, 688, 59), Dark.Object);
        TrainPart(Prefix + TEXT("CeilingLight"), FVector(35, Y, 230), FVector(28, 720, 4), Lamps.Object);
        TrainPart(Prefix + TEXT("Bench"), FVector(-96, Y, 55), FVector(53, 655, 23), Red.Object, true);
        float TrainInfillStart = Y - 388.0f;
        for (int32 Bay = -1; Bay <= 1; ++Bay)
        {
            const float DoorY = DoorCenter(Index, Bay);
            const FString Door = Prefix + FString::Printf(TEXT("Door%d_"), Bay + 2);
            const float TrainInfillEnd = DoorY - DoorHalfWidth;
            const float TrainInfillCenter = (TrainInfillStart + TrainInfillEnd) * 0.5f;
            const float TrainInfillWidth = TrainInfillEnd - TrainInfillStart;
            TrainPart(Door + TEXT("SideWall"), FVector(128, TrainInfillCenter, 125), FVector(14, TrainInfillWidth, 237), Metal.Object, true);
            TrainPart(Door + TEXT("Stripe"), FVector(136, TrainInfillCenter, 70), FVector(4, TrainInfillWidth, 28), Red.Object);
            if (TrainInfillWidth > 100.0f)
            {
                TrainPart(Door + TEXT("PassengerWindow"), FVector(136, TrainInfillCenter, 154), FVector(4, 94, 70), Dark.Object);
            }
            TrainInfillStart = DoorY + DoorHalfWidth;
            TrainPart(Door + TEXT("Lintel"), FVector(138, DoorY, 229), FVector(10, DoorWidth + 2, 16), Silver.Object, true);

            const float ScreenInfillEnd = DoorY - DoorHalfWidth;
            const float ScreenInfillCenter = (ScreenInfillStart + ScreenInfillEnd) * 0.5f;
            const float ScreenInfillWidth = ScreenInfillEnd - ScreenInfillStart;
            AddPart(RootComponent, Door + TEXT("BarrierBase"), FVector(173, ScreenInfillCenter, 57), FVector(13, ScreenInfillWidth, 94), Dark.Object, true);
            AddPart(RootComponent, Door + TEXT("BarrierGlazing"), FVector(173, ScreenInfillCenter, 157), FVector(7, ScreenInfillWidth, 107), Silver.Object, true);
            ScreenInfillStart = DoorY + DoorHalfWidth;
            AddPart(RootComponent, Door + TEXT("BarrierHeader"), FVector(ScreenDoorX, DoorY, 237), FVector(16, DoorWidth + 22, 32), Dark.Object, true);
            UStaticMeshComponent* Blocker = AddPart(RootComponent, Door + TEXT("ClosedBlocker"),
                FVector(ScreenDoorX, DoorY, 115), FVector(9, DoorWidth, 210), Dark.Object, true);
            Blocker->SetVisibility(false);
            Blocker->SetCastShadow(false);
            Blocker->ComponentTags.Add(ClosedDoorBlocker);
            UStaticMeshComponent* Step = AddPart(RootComponent, Door + TEXT("BoardingStep"),
                FVector(165, DoorY, 6), FVector(82, DoorWidth, 8), Silver.Object);
            Step->SetVisibility(false);
            Step->ComponentTags.Add(BoardingStep);
            for (const int32 Side : {-1, 1})
            {
                const FString SideName = Door + (Side > 0 ? TEXT("Right_") : TEXT("Left_"));
                USceneComponent* TrainLeaf = AddLeaf(Train, SideName + TEXT("TrainLeaf"), TrainDoorX, DoorY, Side);
                AddPart(TrainLeaf, SideName + TEXT("TrainPanel"), FVector(0, 0, 115), FVector(5, DoorHalfWidth - 1, 210), Metal.Object);
                AddPart(TrainLeaf, SideName + TEXT("TrainWindow"), FVector(3, 0, 153), FVector(3, DoorHalfWidth - 17, 65), Dark.Object);
                AddPart(RootComponent, SideName + TEXT("ScreenPost"), FVector(ScreenDoorX, DoorY + Side * (DoorHalfWidth + 6), 122), FVector(16, 10, 224), Silver.Object, true);
                USceneComponent* ScreenLeaf = AddLeaf(RootComponent, SideName + TEXT("ScreenLeaf"), ScreenDoorX, DoorY, Side);
                AddPart(ScreenLeaf, SideName + TEXT("ScreenPanel"), FVector(0, 0, 115), FVector(8, DoorHalfWidth - 1, 210), Dark.Object);
                AddPart(ScreenLeaf, SideName + TEXT("ScreenWindow"), FVector(5, 0, 153), FVector(3, DoorHalfWidth - 15, 83), Silver.Object);
            }
            UTextRenderComponent* Sign = CreateDefaultSubobject<UTextRenderComponent>(*(Door + TEXT("BoardingPrompt")));
            Sign->SetupAttachment(RootComponent);
            Sign->SetRelativeLocation(FVector(ScreenDoorX + 9, DoorY, 240));
            Sign->SetHorizontalAlignment(EHTA_Center);
            Sign->SetVerticalAlignment(EVRTA_TextCenter);
            Sign->SetWorldSize(8.5f);
            Sign->SetText(LOCTEXT("Board", "CLICK TO BOARD"));
            Sign->SetVisibility(false);
        }
        TrainPart(Prefix + TEXT("EndSideWall"), FVector(128, (TrainInfillStart + Y + 388.0f) * 0.5f, 125),
            FVector(14, Y + 388.0f - TrainInfillStart, 237), Metal.Object, true);
    }
    AddPart(RootComponent, TEXT("BarrierEndBase"), FVector(173, (ScreenInfillStart + 1470.0f) * 0.5f, 57),
        FVector(13, 1470.0f - ScreenInfillStart, 94), Dark.Object, true);
    AddPart(RootComponent, TEXT("BarrierEndGlazing"), FVector(173, (ScreenInfillStart + 1470.0f) * 0.5f, 157),
        FVector(7, 1470.0f - ScreenInfillStart, 107), Silver.Object, true);
    TrainPart(TEXT("CabRear"), FVector(0,-1178,124), FVector(266,16,233), Red.Object, true);
    TrainPart(TEXT("CabFront"), FVector(0,1178,124), FVector(266,28,233), Red.Object, true);
    TrainPart(TEXT("CabWindshield"), FVector(0,1194,157), FVector(205,8,82), Dark.Object);
    TrainPart(TEXT("CabLeftHeadlight"), FVector(-91,1198,64), FVector(38,7,21), Lamps.Object);
    TrainPart(TEXT("CabRightHeadlight"), FVector(91,1198,64), FVector(38,7,21), Lamps.Object);
}

void ANexoStationRun::BeginPlay()
{
    Super::BeginPlay();
    TArray<USceneComponent*> Components;
    GetComponents(Components);
    for (USceneComponent* Component : Components)
    {
        const bool bPositive = Component->ComponentHasTag(SlidePositive);
        if (bPositive || Component->ComponentHasTag(SlideNegative))
        {
            DoorLeaves.Add({Component, Component->GetRelativeLocation(), (bPositive ? 1.0f : -1.0f) * (DoorHalfWidth + 1.0f)});
        }
        if (UPrimitiveComponent* Primitive = Cast<UPrimitiveComponent>(Component))
        {
            if (Primitive->ComponentHasTag(ClosedDoorBlocker))
            {
                ClosedDoorBlockers.Add(Primitive);
            }
            else if (Primitive->ComponentHasTag(BoardingStep))
            {
                BoardingSteps.Add(Primitive);
            }
        }
    }
    GetComponents(BoardingSigns);
}

void ANexoStationRun::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    const ANexoStationCharacter* Player = Cast<ANexoStationCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));
    if (!Player || bFinished || !Player->HasCrossedFareGate())
    {
        return;
    }

    const double Now = GetWorld()->GetTimeSeconds();
    const FVector Position = Player->GetActorLocation();
    if (TrainStartTime < 0.0 && Position.Z < 118.0 && Position.Y < 350.0 && Position.X > -640.0)
    {
        TrainStartTime = Now;
    }
    if (TrainStartTime < 0.0)
    {
        return;
    }

    if (DoorOpenness < 1.0f)
    {
        const float Elapsed = static_cast<float>(Now - TrainStartTime);
        const float ArrivalProgress = FMath::Clamp(Elapsed / ArrivalSeconds, 0.0f, 1.0f);
        Train->SetRelativeLocation(FVector(0, FMath::InterpEaseOut(StartingY, 0.0f, ArrivalProgress, 2.0f), 0));
        DoorOpenness = FMath::Clamp((Elapsed - ArrivalSeconds) / DoorOpeningSeconds, 0.0f, 1.0f);
        const float SlideProgress = FMath::SmoothStep(0.0f, 1.0f, DoorOpenness);
        for (const FDoorLeaf& Leaf : DoorLeaves)
        {
            Leaf.Component->SetRelativeLocation(Leaf.ClosedPosition + FVector(0, Leaf.SlideDistance * SlideProgress, 0));
        }
        if (DoorOpenness >= 1.0f)
        {
            for (UPrimitiveComponent* Step : BoardingSteps)
            {
                Step->SetCollisionProfileName(TEXT("BlockAll"));
                Step->SetVisibility(true);
            }
            for (UPrimitiveComponent* Blocker : ClosedDoorBlockers)
            {
                Blocker->SetCollisionEnabled(ECollisionEnabled::NoCollision);
            }
            for (UTextRenderComponent* Sign : BoardingSigns)
            {
                Sign->SetVisibility(true);
            }
        }
    }

    const APlayerController* Controller = UGameplayStatics::GetPlayerController(this, 0);
    if (Controller && Controller->WasInputKeyJustPressed(EKeys::LeftMouseButton))
    {
        TryBoardTrain();
    }
}

bool ANexoStationRun::TryBoardTrain()
{
    if (bFinished || DoorOpenness < 1.0f)
    {
        return false;
    }
    const APlayerController* Controller = UGameplayStatics::GetPlayerController(this, 0);
    if (!Controller || !Controller->GetPawn())
    {
        return false;
    }

    FVector WorldView;
    FRotator Aim;
    Controller->GetPlayerViewPoint(WorldView, Aim);
    const FTransform& Transform = GetActorTransform();
    const FVector Origin = Transform.InverseTransformPosition(WorldView);
    const FVector Direction = Transform.InverseTransformVectorNoScale(Aim.Vector());
    const bool bFromPlatform = Origin.X > ScreenDoorX && Direction.X < -0.01f;
    const bool bFromTrain = Origin.X > -125.0f && Origin.X < TrainDoorX && Direction.X > 0.01f;
    if (!bFromPlatform && !bFromTrain)
    {
        return false;
    }

    const float TrainReach = (TrainDoorX - Origin.X) / Direction.X;
    const float ScreenReach = (ScreenDoorX - Origin.X) / Direction.X;
    if (FMath::Max(TrainReach, ScreenReach) > BoardingReach)
    {
        return false;
    }
    const FVector TrainHit = Origin + Direction * TrainReach;
    const FVector ScreenHit = Origin + Direction * ScreenReach;
    if (TrainHit.Z < 10.0f || TrainHit.Z > 220.0f || ScreenHit.Z < 10.0f || ScreenHit.Z > 220.0f)
    {
        return false;
    }

    for (int32 Car = 0; Car < 3; ++Car)
    {
        for (int32 Bay = -1; Bay <= 1; ++Bay)
        {
            const float Center = DoorCenter(Car, Bay);
            if (FMath::Abs(TrainHit.Y - Center) <= DoorHalfWidth && FMath::Abs(ScreenHit.Y - Center) <= DoorHalfWidth)
            {
                FCollisionQueryParams Query(SCENE_QUERY_STAT(BoardTrain), true, Controller->GetPawn());
                FHitResult Obstruction;
                const FVector ExitPoint = Transform.TransformPosition(bFromPlatform ? TrainHit : ScreenHit);
                if (!GetWorld()->LineTraceSingleByChannel(Obstruction, WorldView, ExitPoint, ECC_Visibility, Query))
                {
                    FinishRun();
                    return true;
                }
            }
        }
    }
    return false;
}

void ANexoStationRun::FinishRun()
{
    bFinished = true;
    if (APlayerController* Controller = UGameplayStatics::GetPlayerController(this, 0))
    {
        Controller->SetCinematicMode(true, false, true, true, true);
    }
    if (UGameViewportClient* Viewport = GetWorld()->GetGameViewport())
    {
        CompletionScreen = SNew(SBorder)
            .BorderImage(FCoreStyle::Get().GetBrush("WhiteBrush"))
            .BorderBackgroundColor(FLinearColor(0.014f, 0.023f, 0.035f, 0.92f))
            .HAlign(HAlign_Center)
            .VAlign(VAlign_Center)
            [
                SNew(STextBlock)
                .Text(LOCTEXT("Complete", "Simulation complete"))
                .ColorAndOpacity(FLinearColor::White)
                .Font(FCoreStyle::GetDefaultFontStyle("Bold", 34))
            ];
        Viewport->AddViewportWidgetContent(CompletionScreen.ToSharedRef(), 100);
    }
    UGameplayStatics::SetGamePaused(this, true);
}

void ANexoStationRun::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (CompletionScreen.IsValid())
    {
        if (UGameViewportClient* Viewport = GetWorld()->GetGameViewport())
        {
            Viewport->RemoveViewportWidgetContent(CompletionScreen.ToSharedRef());
        }
        CompletionScreen.Reset();
    }
    Super::EndPlay(EndPlayReason);
}

#undef LOCTEXT_NAMESPACE

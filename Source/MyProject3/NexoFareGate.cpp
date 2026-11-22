#include "NexoFareGate.h"
#include "NexoStationCharacter.h"

#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInterface.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "UObject/ConstructorHelpers.h"

ANexoFareGate::ANexoFareGate()
{
    PrimaryActorTick.bCanEverTick = true;
    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("GatePost"));

    Flap = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Flap"));
    Flap->SetupAttachment(RootComponent);
    Flap->SetRelativeLocation(FVector(39.0, 0.0, 0.0));
    Flap->SetRelativeScale3D(FVector(0.78, 0.045, 0.50));
    Flap->SetCollisionProfileName(TEXT("PhysicsActor"));
    Flap->SetEnableGravity(false);
    Flap->SetLinearDamping(2.0f);
    Flap->SetAngularDamping(2.0f);

    const ConstructorHelpers::FObjectFinder<UStaticMesh> Cube(TEXT("/Engine/BasicShapes/Cube.Cube"));
    Flap->SetStaticMesh(Cube.Object);
    const ConstructorHelpers::FObjectFinder<UMaterialInterface> Teal(
        TEXT("/Game/Station/M_MRT_Teal.M_MRT_Teal"));
    Flap->SetMaterial(0, Teal.Object);
    const ConstructorHelpers::FObjectFinder<UMaterialInterface> Red(
        TEXT("/Game/Station/M_MRT_NorthSouthRed.M_MRT_NorthSouthRed"));
    LockedMaterial = Red.Object;
    const ConstructorHelpers::FObjectFinder<UMaterialInterface> Green(
        TEXT("/Game/Station/M_MRT_Exit.M_MRT_Exit"));
    ReadyMaterial = Green.Object;

    Hinge = CreateDefaultSubobject<UPhysicsConstraintComponent>(TEXT("VerticalHinge"));
    Hinge->SetupAttachment(RootComponent);
    // Chaos uses the constraint's local X axis for twist.
    Hinge->SetRelativeRotation(FRotator(90.0, 0.0, 0.0));
    Hinge->SetLinearXLimit(LCM_Locked, 0.0f);
    Hinge->SetLinearYLimit(LCM_Locked, 0.0f);
    Hinge->SetLinearZLimit(LCM_Locked, 0.0f);
    Hinge->SetAngularSwing1Limit(ACM_Locked, 0.0f);
    Hinge->SetAngularSwing2Limit(ACM_Locked, 0.0f);
    Hinge->SetAngularTwistLimit(ACM_Locked, 0.0f);
}

void ANexoFareGate::BeginPlay()
{
    Super::BeginPlay();
    Flap->SetSimulatePhysics(true);
    Hinge->SetConstrainedComponents(nullptr, NAME_None, Flap, NAME_None);
    if (UStaticMeshComponent* Indicator = StatusIndicator ? StatusIndicator->FindComponentByClass<UStaticMeshComponent>() : nullptr)
    {
        Indicator->SetMaterial(0, LockedMaterial);
    }
}

void ANexoFareGate::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    ANexoStationCharacter* Player = Cast<ANexoStationCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));
    if (!Player || !Player->HasStationTicket())
    {
        return;
    }
    if (!bUnlocked)
    {
        bUnlocked = true;
        Hinge->SetAngularTwistLimit(ACM_Limited, 88.0f);
        if (UStaticMeshComponent* Indicator = StatusIndicator ? StatusIndicator->FindComponentByClass<UStaticMeshComponent>() : nullptr)
        {
            Indicator->SetMaterial(0, ReadyMaterial);
        }
    }
    const FVector LocalPlayer = GetActorTransform().InverseTransformPosition(Player->GetActorLocation());
    const bool InLane = LocalPlayer.X > 3.0 && LocalPlayer.X < 125.0
        && FMath::Abs(LocalPlayer.Y) < 118.0 && FMath::Abs(LocalPlayer.Z) < 125.0;
    if (!InLane)
    {
        OpenAngle = 0.0f;
    }
    else if (OpenAngle == 0.0f)
    {
        OpenAngle = LocalPlayer.Y < 0.0 ? 82.0f : -82.0f;
    }
    if (InLane && OpenAngle > 0.0f && LocalPlayer.Y >= 42.0)
    {
        Player->RecordFareGateCrossing();
    }

    const float CurrentAngle = FMath::FindDeltaAngleDegrees(GetActorRotation().Yaw, Flap->GetComponentRotation().Yaw);
    const float Error = FMath::DegreesToRadians(OpenAngle - CurrentAngle);
    const FVector Axis = GetActorUpVector();
    const float AngularSpeed = FVector::DotProduct(Flap->GetPhysicsAngularVelocityInRadians(), Axis);
    if (FMath::Abs(Error) > 0.008f || FMath::Abs(AngularSpeed) > 0.02f)
    {
        const float Acceleration = FMath::Clamp(68.0f * Error - 13.0f * AngularSpeed, -95.0f, 95.0f);
        Flap->AddTorqueInRadians(Axis * Acceleration, NAME_None, true);
    }
}

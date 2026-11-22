#include "NexoStationCharacter.h"

#include "Animation/AnimSequence.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/SkeletalMesh.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "UObject/ConstructorHelpers.h"

ANexoStationCharacter::ANexoStationCharacter()
{
    PrimaryActorTick.bCanEverTick = true;
    GetCapsuleComponent()->InitCapsuleSize(34.0f, 90.0f);
    bUseControllerRotationYaw = true;
    bUseControllerRotationPitch = false;

    UCharacterMovementComponent* Movement = GetCharacterMovement();
    Movement->MaxWalkSpeed = 350.0f;
    Movement->MaxStepHeight = 32.0f;
    Movement->bOrientRotationToMovement = false;

    FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonView"));
    FirstPersonCamera->SetupAttachment(GetCapsuleComponent());
    FirstPersonCamera->SetRelativeLocation(FVector(19.0, 0.0, 75.0));
    FirstPersonCamera->bUsePawnControlRotation = true;

    const ConstructorHelpers::FObjectFinder<USkeletalMesh> Body(
        TEXT("/Game/Characters/Mannequins/Meshes/SKM_Manny_Simple.SKM_Manny_Simple"));
    GetMesh()->SetSkeletalMeshAsset(Body.Object);
    GetMesh()->SetRelativeLocationAndRotation(FVector(0.0, 0.0, -90.0), FRotator(0.0, -90.0, 0.0));
    GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    const ConstructorHelpers::FObjectFinder<UAnimSequence> Idle(
        TEXT("/Game/Characters/Mannequins/Anims/Unarmed/MM_Idle.MM_Idle"));
    IdleAnimation = Idle.Object;
    const ConstructorHelpers::FObjectFinder<UAnimSequence> Walk(
        TEXT("/Game/Characters/Mannequins/Anims/Unarmed/Walk/MF_Unarmed_Walk_Fwd.MF_Unarmed_Walk_Fwd"));
    WalkAnimation = Walk.Object;
}

void ANexoStationCharacter::BeginPlay()
{
    // The original Blueprint still owns input and kiosk interactions.
    if (UFloatingPawnMovement* LegacyMovement = FindComponentByClass<UFloatingPawnMovement>())
    {
        LegacyMovement->Deactivate();
        LegacyMovement->SetComponentTickEnabled(false);
    }
    TArray<UStaticMeshComponent*> LegacyMeshes;
    GetComponents(LegacyMeshes);
    for (UStaticMeshComponent* LegacyMesh : LegacyMeshes)
    {
        LegacyMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        LegacyMesh->SetVisibility(false);
    }

    if (!HiddenHeadBone.IsNone())
    {
        GetMesh()->HideBoneByName(HiddenHeadBone, PBO_None);
    }
    GetMesh()->PlayAnimation(IdleAnimation, true);
    CurrentAnimation = IdleAnimation;
    Super::BeginPlay();
}

void ANexoStationCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    const float Speed = GetVelocity().Size2D();
    UAnimSequence* DesiredAnimation = Speed > 20.0f ? WalkAnimation : IdleAnimation;
    if (CurrentAnimation != DesiredAnimation)
    {
        GetMesh()->PlayAnimation(DesiredAnimation, true);
        CurrentAnimation = DesiredAnimation;
    }
    GetMesh()->SetPlayRate(DesiredAnimation == WalkAnimation ? FMath::Clamp(Speed / 220.0f, 0.4f, 1.6f) : 1.0f);
}


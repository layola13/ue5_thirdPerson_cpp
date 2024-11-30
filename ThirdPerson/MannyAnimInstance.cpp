#include "MannyAnimInstance.h"

// MannyAnimInstance.cpp
void UMannyAnimInstance::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();

    // Set references to owning character
    AActor *OwningActor = GetOwningActor();
    Character = Cast<ACharacter>(OwningActor);
    if (Character)
    {
        MovementComponent = Character->GetCharacterMovement();
    }
}

void UMannyAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeUpdateAnimation(DeltaSeconds);

    if (!Character || !MovementComponent)
        return;

    // Update velocity and ground speed
    Velocity = MovementComponent->Velocity;
    GroundSpeed = FVector(Velocity.X, Velocity.Y, 0.0f).Size();

    // Update should move state
    const float MovementThreshold = 3.0f;
    FVector CurrentAcceleration = MovementComponent->GetCurrentAcceleration();
    bool bHasAcceleration = !CurrentAcceleration.IsZero();
  // Cache previous falling state
    bool bWasFalling = bIsFalling;

    if (bHasAcceleration)
    {
        UE_LOG(LogTemp, Warning, TEXT("***CurrentAcceleration: %s"), *CurrentAcceleration.ToString());
    }

    // print GroundSpeed

    // print bHasAcceleration

    bShouldMove = (GroundSpeed > MovementThreshold) && bHasAcceleration;

    // Update falling state
    bIsFalling = MovementComponent->IsFalling();

    bCanJump = Velocity.Z > 100 && bIsFalling;

    // Only trigger land when transitioning from falling to not falling
    bLandToLand = bWasFalling && !bIsFalling;
}
#pragma once
#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Logging/LogMacros.h"
#include "MannyAnimInstance.generated.h"



UCLASS()
class UMannyAnimInstance : public UAnimInstance
{
    GENERATED_BODY()

public:
    virtual void NativeInitializeAnimation() override;
    virtual void NativeUpdateAnimation(float DeltaSeconds) override;
public:
    // References
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "References", meta = (BlueprintVisible = "true"))
    ACharacter* Character;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "References",meta = (BlueprintVisible = "true")) 
    UCharacterMovementComponent* MovementComponent;
    
    // Movement
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement",meta = (BlueprintVisible = "true"))
    FVector Velocity;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement",meta = (BlueprintVisible = "true"))
    float GroundSpeed;
    
    // States
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "States",meta = (BlueprintVisible = "true"))
    bool bShouldMove;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "States",meta = (BlueprintVisible = "true"))
    bool bIsFalling;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "States",meta = (BlueprintVisible = "true"))
    bool bCanJump;

   UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "States",meta = (BlueprintVisible = "true"))
    bool bLandToLand;
};
// MannyCustomAnimInstance.h
#pragma once
#include "CoreMinimal.h"
#include "MannyAnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Animation/BlendSpace1D.h"
#include "ControlRig.h"
#include "MannyCustomAnimInstance.generated.h"

UCLASS()
class THIRDPERSON_API UMannyCustomAnimInstance : public UMannyAnimInstance
{
    GENERATED_BODY()

protected:
    // Animation Assets
    UPROPERTY(EditDefaultsOnly, Category = "Animation")
    UBlendSpace1D* WalkRunBlendSpace;

    UPROPERTY(EditDefaultsOnly, Category = "Animation")
    UAnimSequence* IdleAnim;

    UPROPERTY(EditDefaultsOnly, Category = "Animation")
    UAnimSequence* JumpAnim;

    UPROPERTY(EditDefaultsOnly, Category = "Animation")
    UAnimSequence* FallLoopAnim;

    UPROPERTY(EditDefaultsOnly, Category = "Animation")
    UAnimSequence* LandAnim;

    // Control Rig
    UPROPERTY(EditDefaultsOnly, Category = "Animation|ControlRig")
    TSubclassOf<UControlRig> FootIKControlRigClass;

    UPROPERTY()
    UControlRig* FootIKControlRig;

private:
    // Montage Caching
    UPROPERTY()
    TMap<UAnimSequence*, UAnimMontage*> SequenceMontageCache;
    
    UPROPERTY()
    TMap<UBlendSpace1D*, UAnimMontage*> BlendSpaceMontageCache;
    
    UPROPERTY()
    UAnimMontage* CurrentMontage;

    // Blend Settings
    const float DefaultBlendInTime = 0.25f;
    const float DefaultBlendOutTime = 0.25f;
    const float DefaultPlayRate = 1.0f;

protected:
    void PlaySequence(UAnimSequence* Animation, bool bLoop, float BlendInTime = 0.25f);
    void PlayBlendSpace(UBlendSpace1D* BlendSpace, float Speed, float BlendInTime = 0.25f);
    void StopAnimation(float BlendOutTime = 0.25f);
    
private:
    UAnimMontage* GetOrCreateSequenceMontage(UAnimSequence* Animation);
    UAnimMontage* GetOrCreateBlendSpaceMontage(UBlendSpace1D* BlendSpace);

public:
    virtual void NativeInitializeAnimation() override;
    virtual void NativeUpdateAnimation(float DeltaSeconds) override;
};
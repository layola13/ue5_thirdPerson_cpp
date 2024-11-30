#include "MannyCustomAnimInstance.h"

// MannyCustomAnimInstance.cpp

// MannyCustomAnimInstance.cpp
UAnimMontage *UMannyCustomAnimInstance::GetOrCreateSequenceMontage(UAnimSequence *Animation)
{
    if (!Animation)
        return nullptr;

    // 检查缓存
    if (UAnimMontage **CachedMontage = SequenceMontageCache.Find(Animation))
    {
        return *CachedMontage;
    }

    // 创建新蒙太奇
    FSlotAnimationTrack SlotTrack;
    SlotTrack.SlotName = FName("DefaultSlot");

    // Create segment properly
    FAnimSegment Segment;
    Segment.AnimReference = Animation;
    Segment.StartPos = 0.0f;
    Segment.AnimStartTime = 0.0f;
    Segment.AnimEndTime = Animation->GetPlayLength();
    SlotTrack.AnimTrack.AnimSegments.Add(Segment);

    UAnimMontage *Montage = NewObject<UAnimMontage>();
    Montage->SlotAnimTracks.Add(SlotTrack);

    // 缓存并返回
    SequenceMontageCache.Add(Animation, Montage);
    return Montage;
}

// Add to MannyCustomAnimInstance.cpp
UAnimMontage* UMannyCustomAnimInstance::GetOrCreateBlendSpaceMontage(UBlendSpace1D* BlendSpace)
{
    if (!BlendSpace) return nullptr;

    // Check cache
    if (UAnimMontage** CachedMontage = BlendSpaceMontageCache.Find(BlendSpace))
    {
        return *CachedMontage;
    }

    // Create new montage
    FSlotAnimationTrack SlotTrack;
    SlotTrack.SlotName = FName("DefaultSlot");

    // Create segment for blend space
    FAnimSegment Segment;
    Segment.SetAnimReference(Cast<UAnimSequenceBase>(BlendSpace));
    Segment.StartPos = 0.0f;
    Segment.AnimStartTime = 0.0f;
    Segment.AnimEndTime = BlendSpace->GetPlayLength();
    SlotTrack.AnimTrack.AnimSegments.Add(Segment);

    UAnimMontage* Montage = NewObject<UAnimMontage>();
    Montage->SlotAnimTracks.Add(SlotTrack);

    // Cache and return
    BlendSpaceMontageCache.Add(BlendSpace, Montage);
    return Montage;
}
void UMannyCustomAnimInstance::PlaySequence(UAnimSequence *Animation, bool bLoop, float BlendInTime)
{
    if (!Animation)
        return;

    UAnimMontage *Montage = GetOrCreateSequenceMontage(Animation);
    if (!Montage)
        return;

    // 设置混合参数
    Montage->BlendIn.SetBlendTime(BlendInTime);
    Montage->BlendOut.SetBlendTime(DefaultBlendOutTime);

    // 播放蒙太奇
    float Duration = Montage_Play(Montage, DefaultPlayRate);

    Montage->bLoop = bLoop;
    CurrentMontage = Montage;
}

void UMannyCustomAnimInstance::PlayBlendSpace(UBlendSpace1D *BlendSpace, float Speed, float BlendInTime)
{
     if (!BlendSpace) return;

    UAnimMontage* Montage = GetOrCreateBlendSpaceMontage(BlendSpace);
    if (!Montage) return;

    FMontageBlendSettings BlendSettings(BlendInTime);
    BlendSettings.BlendMode = EMontageBlendMode::Standard;

    // 正确的方式：使用BlendSpace的参数设置
    const FBlendParameter& SpeedParameter = BlendSpace->GetBlendParameter(0);
    float ClampedSpeed = FMath::Clamp(Speed, SpeedParameter.Min, SpeedParameter.Max);
    
    // Set input value
    FVector BlendInput(ClampedSpeed, 0.0f, 0.0f);
    BlendInput = BlendSpace->GetClampedAndWrappedBlendInput(BlendInput);

    Montage_Play(Montage, DefaultPlayRate);
    CurrentMontage = Montage;
}
void UMannyCustomAnimInstance::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();

    // Initialize Control Rig
    if (FootIKControlRigClass)
    {
        FootIKControlRig = NewObject<UControlRig>(this, FootIKControlRigClass);
        if (FootIKControlRig)
        {
            FootIKControlRig->Initialize(true);
        }
    }
}

void UMannyCustomAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeUpdateAnimation(DeltaSeconds);

    if (!Character || !MovementComponent)
        return;

    // Update Control Rig
    if (FootIKControlRig)
    {
        bool bEnableIK = !bIsFalling && !bCanJump;
        FootIKControlRig->SetControlValue<bool>(FName("bShouldDoIK"), bEnableIK);
        FootIKControlRig->SetControlValue<float>(FName("Alpha"), bEnableIK ? 1.0f : 0.0f);
        FootIKControlRig->Execute(TEXT("Execute"));
    }

    // Update Animation States
    if (bIsFalling)
    {
        if (bCanJump)
            PlaySequence(JumpAnim, false, 0.1f);
        else
            PlaySequence(FallLoopAnim, true, 0.2f);
    }
    else if (bLandToLand)
    {
        PlaySequence(LandAnim, false, 0.1f);
    }
    else if (bShouldMove)
    {
        PlayBlendSpace(WalkRunBlendSpace, GroundSpeed, 0.25f);
    }
    else if (!bShouldMove && CurrentMontage != GetOrCreateSequenceMontage(IdleAnim))
    {
        PlaySequence(IdleAnim, true, 0.25f);
    }
}

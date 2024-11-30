// FootIKControlRig.cpp
#include "FootIKControlRig.h"

UFootIKControlRig::UFootIKControlRig(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    Source = FTransform::Identity;
    Alpha = 1.0f;
    bShouldDoIK = true;
}

void UFootIKControlRig::Initialize(bool bInInitRigUnits)
{
    Super::Initialize(bInInitRigUnits);
    
    // Additional initialization if needed
}

void UFootIKControlRig::Evaluate_AnyThread()
{
    Super::Evaluate_AnyThread();

    URigHierarchy* Hierarchy = GetHierarchy();
    if (!Hierarchy || Alpha <= 0.0f) return;

    if (bShouldDoIK)
    {
        // Get foot locations
        FVector LeftOffset = FootTrace(Hierarchy, LeftFootBone);
        FVector RightOffset = FootTrace(Hierarchy, RightFootBone);

        // Apply alpha blending
        ZOffset_L = LeftOffset.Z * Alpha;
        ZOffset_R = RightOffset.Z * Alpha;
        ZOffset_Pelvis = FMath::Min(ZOffset_L, ZOffset_R) * Alpha;

        // Apply transforms
        ModifyTransforms(Hierarchy, LeftFootBone, ZOffset_L);
        ModifyTransforms(Hierarchy, RightFootBone, ZOffset_R);
        ModifyTransforms(Hierarchy, PelvisBone, ZOffset_Pelvis);

        // Apply source transform if provided
        if (!Source.Equals(FTransform::Identity))
        {
            FTransform CurrentTransform = Hierarchy->GetGlobalTransform(FRigElementKey(PelvisBone, ERigElementType::Bone));
            FTransform BlendedTransform = FTransform::Identity;
            BlendedTransform.Blend(BlendedTransform, Source, Alpha);
            Hierarchy->SetGlobalTransform(FRigElementKey(PelvisBone, ERigElementType::Bone), BlendedTransform * CurrentTransform, true);
        }
    }
}

bool UFootIKControlRig::Execute(const FName& InEventName)
{
    if (!Super::Execute(InEventName))
    {
        return false;
    }

    if (InEventName == TEXT("Execute"))
    {
        Evaluate_AnyThread();
    }

    return true;
}

bool UFootIKControlRig::Execute_Internal(const FName& InEventName)
{
    if (!Super::Execute_Internal(InEventName))
    {
        return false;
    }

    return true;
}

FVector UFootIKControlRig::FootTrace(const URigHierarchy* Hierarchy, const FName& FootBone)
{
    FTransform FootTransform = Hierarchy->GetGlobalTransform(FRigElementKey(FootBone, ERigElementType::Bone));
    FVector Start = FootTransform.GetLocation() + FVector(0, 0, TraceUpOffset);
    FVector End = Start - FVector(0, 0, TraceDownDistance);
    
    FHitResult HitResult;
    FCollisionQueryParams QueryParams;
    QueryParams.bTraceComplex = false;
    FCollisionShape SphereShape = FCollisionShape::MakeSphere(SphereRadius);
    
    if (GetWorld()->SweepSingleByChannel(
        HitResult,
        Start, End,
        FQuat::Identity,
        ECC_Visibility,
        SphereShape,
        QueryParams))
    {
        return HitResult.ImpactPoint - FootTransform.GetLocation();
    }
    
    return FVector::ZeroVector;
}

void UFootIKControlRig::ModifyTransforms(URigHierarchy* Hierarchy, const FName& BoneName, float ZOffset)
{
    FTransform Transform = Hierarchy->GetGlobalTransform(FRigElementKey(BoneName, ERigElementType::Bone));
    Transform.AddToTranslation(FVector(0, 0, ZOffset));
    Hierarchy->SetGlobalTransform(FRigElementKey(BoneName, ERigElementType::Bone), Transform, false);
}
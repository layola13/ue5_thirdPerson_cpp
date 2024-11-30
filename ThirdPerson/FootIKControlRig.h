// FootIKControlRig.h
#pragma once
#include "CoreMinimal.h"
#include "ControlRig.h"
#include "RigVMCore/RigVM.h"
#include "Units/RigUnitContext.h"
#include "Units/Execution/RigUnit_Hierarchy.h"
#include "FootIKControlRig.generated.h"

class FRigHierarchy;
UCLASS(Blueprintable, meta=(DisplayName="Basic Foot IK"))
class  UFootIKControlRig : public UControlRig 
{
    GENERATED_UCLASS_BODY()

public:
    // Blueprint exposed parameters
    UPROPERTY(EditAnywhere, Category="Inputs", meta=(PinShownByDefault))
    FTransform Source;

    UPROPERTY(EditAnywhere, Category="Inputs", meta=(PinShownByDefault, ClampMin="0.0", ClampMax="1.0"))
    float Alpha = 1.0f;

    UPROPERTY(EditAnywhere, Category="Inputs", meta=(PinShownByDefault))
    bool bShouldDoIK = true;

protected:
    // Official API overrides
    virtual void Initialize(bool bInInitRigUnits = true) override;
    virtual void Evaluate_AnyThread() override;
    virtual bool Execute(const FName& InEventName) override;
    virtual bool Execute_Internal(const FName& InEventName) override;

private:
    // Change parameter types to match URigHierarchy
    FVector FootTrace(const URigHierarchy* Hierarchy, const FName& FootBone); 
    void ModifyTransforms(URigHierarchy* Hierarchy, const FName& BoneName, float ZOffset);

    const FName LeftFootBone = TEXT("IK_foot_L");
    const FName RightFootBone = TEXT("IK_foot_R");
    const FName PelvisBone = TEXT("pelvis");

    float ZOffset_L = 0.0f;
    float ZOffset_R = 0.0f;
    float ZOffset_Pelvis = 0.0f;

      // Add trace parameters
    UPROPERTY(EditAnywhere, Category = "Trace Settings")
    float TraceUpOffset = 30.0f;

    UPROPERTY(EditAnywhere, Category = "Trace Settings") 
    float TraceDownDistance = 50.0f;

    UPROPERTY(EditAnywhere, Category = "Trace Settings")
    float SphereRadius = 3.0f;
};
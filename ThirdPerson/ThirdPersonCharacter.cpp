// Copyright Epic Games, Inc. All Rights Reserved.

#include "ThirdPersonCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// AThirdPersonCharacter

// 发射多发子弹
// void FireMultipleBullets(int32 BulletCount)
// {
//     FVector MuzzleLocation = GetMuzzleLocation();
//     FRotator MuzzleRotation = GetMuzzleRotation();

//     // 生成多发子弹
//     for (int32 i = 0; i < BulletCount; i++)
//     {
//         // 添加一些随机扩散
//         FRotator SpreadRotation = MuzzleRotation;
//         SpreadRotation.Pitch += FMath::RandRange(-2.0f, 2.0f);
//         SpreadRotation.Yaw += FMath::RandRange(-2.0f, 2.0f);

//         FActorSpawnParameters SpawnParams;
//         SpawnParams.Owner = this;
//         SpawnParams.Instigator = GetInstigator();

//         // 生成子弹
//         GetWorld()->SpawnActor<AProjectile>(ProjectileClass, 
//             MuzzleLocation, 
//             SpreadRotation, 
//             SpawnParams);
//     }
// }

AThirdPersonCharacter::AThirdPersonCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true;			 // Character moves in the direction of input...
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// // Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom2024"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;		// The camera follows at this distance behind the character
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// // Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera2024"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false;								// Camera does not rotate relative to arm

    
	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character)
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera2024"));

	FirstPersonCamera->SetupAttachment(GetCapsuleComponent());
	FirstPersonCamera->SetRelativeLocation(FVector(10.f, 0.f, 100.f)); // Position the camera

	FirstPersonCamera->bUsePawnControlRotation = true;

	// activiate is false

	// Set default view mode
	CurrentViewMode = EPlayerViewMode::ThirdPerson;

	FirstPersonCamera->SetActive(false);
}

void AThirdPersonCharacter::BeginPlay()
{
	// Call the base class
	Super::BeginPlay();

	if (CameraBoom)
	{
		CameraBoom->bUsePawnControlRotation = true;
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("CameraBoom not found"));
	}

	if (FollowCamera)
	{
		FollowCamera->bUsePawnControlRotation = false;
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("FollowCamera not found"));
	}

	if (FirstPersonCamera)
	{
		// print found camera

		UE_LOG(LogTemplateCharacter, Log, TEXT("Found Camera"));
	}
	else
	{
		// print not found camera
		UE_LOG(LogTemplateCharacter, Error, TEXT("Camera not found"));
	}
}

//////////////////////////////////////////////////////////////////////////
// Input

void AThirdPersonCharacter::SetupPlayerInputComponent(UInputComponent *PlayerInputComponent)
{
	// Add Input Mapping Context
	if (APlayerController *PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem *Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	// Set up action bindings
	if (UEnhancedInputComponent *EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{

		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AThirdPersonCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AThirdPersonCharacter::Look);

		// ChangeAction

		EnhancedInputComponent->BindAction(ChangeAction, ETriggerEvent::Started, this, &AThirdPersonCharacter::ChangeCamera);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AThirdPersonCharacter::ChangeCamera(const FInputActionValue &Value)
{

	UE_LOG(LogTemplateCharacter, Log, TEXT("Change Camera"));

	// if (FirstPersonCamera->IsActive())
	// {
	// 	FollowCamera->SetActive(true);
	// 	FirstPersonCamera->SetActive(false);
	// }
	// else
	// {
	// 	FollowCamera->SetActive(false);
	// 	FirstPersonCamera->SetActive(true);
	// }

	// switch current view mode

	// 获取枚举的总数量
	UEnum *EnumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("EPlayerViewMode"), true);
	if (!EnumPtr)
		return;

	int32 EnumCount = EnumPtr->NumEnums() - 1; // 减去1，因为最后一个是无效的

	CurrentViewMode = static_cast<EPlayerViewMode>((static_cast<uint8>(CurrentViewMode) + 1) % static_cast<uint8>(EnumCount));

	switch (CurrentViewMode)
	{
	case EPlayerViewMode::ThirdPerson:

		FollowCamera->SetActive(true);

		FirstPersonCamera->SetActive(true);

		CameraBoom->TargetArmLength = 400.0f;

		// reset camera boom rotation

		CameraBoom->SetRelativeRotation(FRotator(0.f, 0.f, 0.f));

		break;

	case EPlayerViewMode::FirstPerson:

		FollowCamera->SetActive(false);

		FirstPersonCamera->SetActive(true);

		break;

	case EPlayerViewMode::TopDown:

		FollowCamera->SetActive(true);

		FirstPersonCamera->SetActive(false);

		CameraBoom->TargetArmLength = 800.f;
		CameraBoom->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));

		break;

	default:
		break;
	}
}

void AThirdPersonCharacter::Move(const FInputActionValue &Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AThirdPersonCharacter::Look(const FInputActionValue &Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}
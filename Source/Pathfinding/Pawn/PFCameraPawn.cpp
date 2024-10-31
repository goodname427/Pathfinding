// Fill out your copyright notice in the Description page of Project Settings.


#include "PFCameraPawn.h"
#include "Kismet/GameplayStatics.h"
#include "PFUtils.h"

// Sets default values
APFCameraPawn::APFCameraPawn(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	InitDefaultSubobject(RootComponent);

	// Spring Arm
	InitDefaultSubobject(SpringArm);
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, 50.0f), FRotator(-60.0f, 0.0f, 0.0f));
	SpringArm->TargetArmLength = 3000.f;
	SpringArm->bEnableCameraLag = true;
	SpringArm->CameraLagSpeed = 3.0f;

	// Camera
	InitDefaultSubobject(Camera);
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);

	// Movement
	Movement = CreateDefaultSubobject<UPawnMovementComponent, UPFCameraPawnMovementComponent>("Movement");
	Movement->UpdatedComponent = RootComponent;

	// Camera Scale
	MinTargetArmLength = 3000.f;
	MaxTargetArmLength = 5000.f;
	CameraScaleSpeed = 0.1f;
	CameraScaleValue = 0.5f;

	// Mouse Move
	bEnableMouseMove = true;

	// Camera Rotate
	bUseControllerRotationYaw = true;
	CameraRotateSpeed = 100.f;
	bControlPressed = false;

	// Flag
	InitDefaultSubobject(StaticMesh);
	StaticMesh->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void APFCameraPawn::BeginPlay()
{
	Super::BeginPlay();

	SpringArm->TargetArmLength = FMath::Lerp(MinTargetArmLength, MaxTargetArmLength, 0.5f);
	CameraScaleValue = 0.5f;
}

// Called every frame
void APFCameraPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Camera Arm Lerp
	SpringArm->TargetArmLength = FMath::Lerp(SpringArm->TargetArmLength, FMath::Lerp(MinTargetArmLength, MaxTargetArmLength, CameraScaleValue), DeltaTime * 4.f);

	// Mouse Move
	if (bEnableMouseMove)
	{
		FVector2D MousePositionOnEdge;
		if (IsMouseOnScreenEdge(MousePositionOnEdge))
		{
			MoveVertical(MousePositionOnEdge.X);
			MoveHorizontal(MousePositionOnEdge.Y);
		}
	}
}

// Called to bind functionality to input
void APFCameraPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	FastBindAxis(MoveVertical);
	FastBindAxis(MoveHorizontal);
	FastBindAxis(CameraScale);
	FastBindAction(Control);
	FastBindAxis(MouseHorizontal);
}

bool APFCameraPawn::IsMouseOnScreenEdge(FVector2D& OutMousePositionOnEdge)
{
	FVector2D MousePosition;
	GEngine->GameViewport->GetMousePosition(MousePosition);
	FVector2D ViewportSize;
	GEngine->GameViewport->GetViewportSize(ViewportSize);

	// UE_LOG_TEMP(TEXT("Viewport Size: (%.2f, %.2f), Mouse Position: (%.2f, %.2f)"), ViewportSize.X, ViewportSize.Y, MousePosition.X, MousePosition.Y);

	OutMousePositionOnEdge.X = OutMousePositionOnEdge.Y = 0;
	OutMousePositionOnEdge.X = MousePosition.Y == 0 ? 1 : (MousePosition.Y >= ViewportSize.Y - 1 ? -1 : 0);
	OutMousePositionOnEdge.Y = MousePosition.X == 0 ? -1 : (MousePosition.X >= ViewportSize.X - 1 ? 1 : 0);

	return OutMousePositionOnEdge.X != 0 || OutMousePositionOnEdge.Y != 0;
}

void APFCameraPawn::Move(float Value, EAxis::Type InAxis)
{
	if (Controller == nullptr)
	{
		return;
	}

	FVector Direction = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(InAxis);
	AddMovementInput(Direction, Value);
}

void APFCameraPawn::MoveVertical(float Value)
{
	Move(Value, EAxis::X);
}

void APFCameraPawn::MoveHorizontal(float Value)
{
	Move(Value, EAxis::Y);
}

void APFCameraPawn::ControlPressed()
{
	bControlPressed = true;
}

void APFCameraPawn::ControlReleased()
{
	bControlPressed = false;
}

void APFCameraPawn::MouseHorizontal(float Value)
{
	if (bControlPressed)
	{
		// UE_LOG_TEMP(TEXT("Mouse Horizontal: %f"), Value);
		AddControllerYawInput(Value * CameraRotateSpeed * GetWorld()->GetDeltaSeconds());
	}
}

void APFCameraPawn::CameraScale(float Value)
{
	// UE_LOG_TEMP(TEXT("Camera Scale: %f"), Value);
	CameraScaleValue = FMath::Clamp(CameraScaleValue + Value * CameraScaleSpeed, 0.f, 1.f);

}


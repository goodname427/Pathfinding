// Fill out your copyright notice in the Description page of Project Settings.


#include "CommanderPawn.h"
#include "Kismet/GameplayStatics.h"
#include "PFUtils.h"

// Sets default values
ACommanderPawn::ACommanderPawn()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	INIT_DEFAULT_SUBOBJECT(RootComponent);

	// Spring Arm
	INIT_DEFAULT_SUBOBJECT(SpringArm);
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, 50.0f), FRotator(-60.0f, 0.0f, 0.0f));
	SpringArm->TargetArmLength = 3000.f;
	SpringArm->bEnableCameraLag = true;
	SpringArm->CameraLagSpeed = 3.0f;

	// Camera
	INIT_DEFAULT_SUBOBJECT(Camera);
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);

	// Movement
	Movement = CreateDefaultSubobject<UPawnMovementComponent, UCommanderPawnMovementComponent>("Movement");
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
	INIT_DEFAULT_SUBOBJECT(StaticMesh);
	StaticMesh->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ACommanderPawn::BeginPlay()
{
	Super::BeginPlay();

	SpringArm->TargetArmLength = FMath::Lerp(MinTargetArmLength, MaxTargetArmLength, 0.5f);
	CameraScaleValue = 0.5f;
}

// Called every frame
void ACommanderPawn::Tick(float DeltaTime)
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
void ACommanderPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	FAST_BIND_AXIS(MoveVertical);
	FAST_BIND_AXIS(MoveHorizontal);
	FAST_BIND_AXIS(CameraScale);
	FAST_BIND_ACTION(Control);
	FAST_BIND_AXIS(MouseHorizontal);
}

bool ACommanderPawn::IsMouseOnScreenEdge(FVector2D& OutMousePositionOnEdge)
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

void ACommanderPawn::Move(float Value, EAxis::Type InAxis)
{
	if (Controller == nullptr)
	{
		return;
	}

	FVector Direction = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(InAxis);
	AddMovementInput(Direction, Value);
}

void ACommanderPawn::MoveVertical(float Value)
{
	Move(Value, EAxis::X);
}

void ACommanderPawn::MoveHorizontal(float Value)
{
	Move(Value, EAxis::Y);
}

void ACommanderPawn::ControlPressed()
{
	bControlPressed = true;
}

void ACommanderPawn::ControlReleased()
{
	bControlPressed = false;
}

void ACommanderPawn::MouseHorizontal(float Value)
{
	if (bControlPressed)
	{
		// UE_LOG_TEMP(TEXT("Mouse Horizontal: %f"), Value);
		AddControllerYawInput(Value * CameraRotateSpeed * GetWorld()->GetDeltaSeconds());
	}
}

void ACommanderPawn::CameraScale(float Value)
{
	// UE_LOG_TEMP(TEXT("Camera Scale: %f"), Value);
	CameraScaleValue = FMath::Clamp(CameraScaleValue + Value * CameraScaleSpeed, 0.f, 1.f);

}


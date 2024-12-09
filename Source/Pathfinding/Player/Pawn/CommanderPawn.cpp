// Fill out your copyright notice in the Description page of Project Settings.


#include "CommanderPawn.h"

#include "PFBlueprintFunctionLibrary.h"
#include "PFHUD.h"
#include "PFPlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "PFUtils.h"
#include "Net/UnrealNetwork.h"

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

	// Select
	LineTraceDistance = 100 * 100.f; // 100 m
	LineTraceStep = 0.01f;
	SingleSelectPressedDuration = 0.1f;
	SingleSelectBoxSize = 10.f;

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
	SpringArm->TargetArmLength = FMath::Lerp(SpringArm->TargetArmLength,
	                                         FMath::Lerp(MinTargetArmLength, MaxTargetArmLength, CameraScaleValue),
	                                         DeltaTime * 4.f);

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

void ACommanderPawn::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, SelectedPawns);
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
	FAST_BIND_ACTION(Select);
	FAST_BIND_ACTION(Select_Ctrl);
	FAST_BIND_ACTION(Target);

	PlayerInputComponent->BindAction("Test", IE_Pressed, this, &ThisClass::Test);
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

void ACommanderPawn::SelectPressed()
{
	BeginSelect();
}

void ACommanderPawn::SelectReleased()
{
	EndSelect(false);
}

void ACommanderPawn::Select_CtrlPressed()
{
	BeginSelect();
}

void ACommanderPawn::Select_CtrlReleased()
{
	EndSelect(true);
}

void ACommanderPawn::BeginSelect()
{
	bSelectPressed = true;
	SelectPressedTime = GetWorld()->GetTimeSeconds();

	APlayerController* PlayerController = GetController<APlayerController>();
	if (PlayerController != nullptr)
	{
		APFHUD* HUD = PlayerController->GetHUD<APFHUD>();
		if (HUD != nullptr)
		{
			HUD->BeginDrawSelectBox();
		}
	}
}

void ACommanderPawn::EndSelect(bool bAddtional)
{
	bSelectPressed = false;
	float Duration = GetWorld()->GetTimeSeconds() - SelectPressedTime;

	APlayerController* PlayerController = GetController<APlayerController>();
	if (PlayerController != nullptr)
	{
		APFHUD* HUD = PlayerController->GetHUD<APFHUD>();
		if (HUD != nullptr)
		{
			FBox2D SelectBox = HUD->EndDrawSelectBox();

			if (!bAddtional)
			{
				// deselected
				for (APFPawn* PFPawn : SelectedPawns)
				{
					PFPawn->OnDeselected();
				}
				SelectedPawns.Reset();
			}

			// selected single or multi
			FVector2D SelectBoxSize = SelectBox.GetSize();
			if (Duration < SingleSelectPressedDuration || SelectBoxSize.X * SelectBoxSize.Y < SingleSelectBoxSize)
			{
				// single select can select other's pawn (to view information of the pawn), but multi select can not
				SingleSelect(PlayerController, SelectBox.GetCenter(), false);
			}
			else
			{
				MultiSelect(PlayerController, SelectBox);
			}

			// DEBUG_MESSAGE(TEXT("Selected Actors [%d]"), SelectedPawns.Num());
			// for (AActor* Actor : SelectedPawns)
			// {
			// 	DEBUG_MESSAGE(TEXT("Selected Actor [%s]"), *Actor->GetName());
			// }
		}
	}
}

void ACommanderPawn::SingleSelect(APlayerController* PlayerController, const FVector2D& MousePos, bool bMustBeSelf)
{
	static TArray<FHitResult> TempHitResults;
	TempHitResults.Reset();

	LineTrace(TempHitResults, PlayerController, MousePos);
	for (FHitResult HitResult : TempHitResults)
	{
		APFPawn* PFPawn = Cast<APFPawn>(HitResult.Actor.Get());
		if (PFPawn != nullptr && (!bMustBeSelf || PFPawn->GetOwnerPlayer() == GetPlayerState<APFPlayerState>()) && !
			PFPawn->HasSelected())
		{
			SelectedPawns.Add(PFPawn);
			PFPawn->OnSelected(this);
		}
	}
}

void ACommanderPawn::MultiSelect(APlayerController* PlayerController, const FBox2D& SelectBox)
{
	int XStep, YStep;
	PlayerController->GetViewportSize(XStep, YStep);
	XStep = FMath::CeilToInt(XStep * LineTraceStep);
	YStep = FMath::CeilToInt(YStep * LineTraceStep);

	for (float X = SelectBox.Min.X; X <= SelectBox.Max.X; X += XStep)
	{
		for (float Y = SelectBox.Min.Y; Y <= SelectBox.Max.Y; Y += YStep)
		{
			SingleSelect(PlayerController, FVector2D(X, Y), true);
		}
	}
}

void ACommanderPawn::LineTrace(TArray<FHitResult>& OutHits, APlayerController* Player, FVector2D ScreenPoint)
{
	static FVector LineStart, LineEnd;
	UGameplayStatics::DeprojectScreenToWorld(Player, ScreenPoint, LineStart, LineEnd);
	LineEnd = LineStart + LineEnd * LineTraceDistance;

	GetWorld()->LineTraceMultiByChannel(OutHits, LineStart, LineEnd, ECollisionChannel::ECC_Visibility);
}

void ACommanderPawn::TargetPressed()
{
}

void ACommanderPawn::TargetReleased()
{
}

void ACommanderPawn::SpawnPFPawn_Implementation(TSubclassOf<APFPawn> PawnClass, FVector Location)
{
	UPFBlueprintFunctionLibrary::SpawnPFPawn(this, PawnClass, this, Location, FRotator(0, 0, 0));
}

void ACommanderPawn::Test_Implementation()
{
}

void ACommanderPawn::CameraScale(float Value)
{
	// UE_LOG_TEMP(TEXT("Camera Scale: %f"), Value);
	CameraScaleValue = FMath::Clamp(CameraScaleValue + Value * CameraScaleSpeed, 0.f, 1.f);
}

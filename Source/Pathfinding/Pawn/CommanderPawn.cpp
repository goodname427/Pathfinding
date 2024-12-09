// Fill out your copyright notice in the Description page of Project Settings.


#include "CommanderPawn.h"

#include "BattleHUD.h"
#include "PFBlueprintFunctionLibrary.h"
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

	// Step = (ViewportSizeY * PawnMinSize) / (2 * tan(FOV / 2) * ArmLength)
	// Step = (ViewportSizeY / ArmLength) * (PawnMinSize / 2 * tan(FOV / 2))
	// Factor = PawnMinSizeY / (2 * tan(FOV / 2))
	// Step = (ViewportSizeY / ArmLength) * Factor

	// SubFactor = ViewportSizeY / ArmLength
	// Step = Factor * SubFactor

	// FOV = 90d
	// FactorXY = PawnMinSize / 2

	LineTraceStepFactor = GetDefault<UPFGameSettings>()->PawnMinSize * 0.5f;
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
	PlayerInputComponent->BindAction("Select", IE_DoubleClick, this, &ThisClass::SelectDoubleClick);
	FAST_BIND_ACTION(Target);

	PlayerInputComponent->BindAction("Test", IE_Pressed, this, &ThisClass::Test);
}

bool ACommanderPawn::IsMouseOnScreenEdge(FVector2D& OutMousePositionOnEdge) const
{
	APlayerController* PlayerController = GetController<APlayerController>();
	if (PlayerController == nullptr)
	{
		return false;
	}

	FVector2D MousePosition;
	PlayerController->GetMousePosition(MousePosition.X, MousePosition.Y);
	int32 ViewportSizeX, ViewportSizeY;
	PlayerController->GetViewportSize(ViewportSizeX, ViewportSizeY);

	// UE_LOG_TEMP(TEXT("Viewport Size: (%.2f, %.2f), Mouse Position: (%.2f, %.2f)"), ViewportSize.X, ViewportSize.Y, MousePosition.X, MousePosition.Y);

	OutMousePositionOnEdge.X = OutMousePositionOnEdge.Y = 0;
	OutMousePositionOnEdge.X = MousePosition.Y == 0 ? 1 : (MousePosition.Y >= ViewportSizeY - 1 ? -1 : 0);
	OutMousePositionOnEdge.Y = MousePosition.X == 0 ? -1 : (MousePosition.X >= ViewportSizeX - 1 ? 1 : 0);

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
	bDoubleClick = false;
	BeginSelect();
}

void ACommanderPawn::SelectReleased()
{
	EndSelect(false, bDoubleClick);
}

void ACommanderPawn::Select_CtrlPressed()
{
	BeginSelect();
}

void ACommanderPawn::Select_CtrlReleased()
{
	EndSelect(true);
}

void ACommanderPawn::SelectDoubleClick()
{
	// double-click a pawn to select all pawns on the screen which are of the same class as the clicked pawn 
	bDoubleClick = true;
	APlayerController* PlayerController = GetController<APlayerController>();
	if (PlayerController != nullptr)
	{
		static FVector2D MousePosition;
		PlayerController->GetMousePosition(MousePosition.X, MousePosition.Y);

		FHitResult HitResult;
		LineTrace(PlayerController, MousePosition, HitResult);
		APFPawn* ClickedPawn = Cast<APFPawn>(HitResult.Actor.Get());
		
		if (ClickedPawn != nullptr && IsOwned(ClickedPawn))
		{
			int32 ViewportSizeX, ViewportSizeY;
			PlayerController->GetViewportSize(ViewportSizeX, ViewportSizeY);

			static FMultiLineTraceResult Result;
			SelectBoxLineTracePawn(PlayerController, {{0, 0}, FVector2D(ViewportSizeX, ViewportSizeY)}
			                       , Result);

			DeselectAll();
			for (APFPawn* HitPawn : Result.HitPawns)
			{
				if (HitPawn->GetClass() == ClickedPawn->GetClass() && IsOwned(HitPawn))
				{
					Select(HitPawn);
				}
			}
		}
	}
}

void ACommanderPawn::BeginSelect()
{
	bSelectPressed = true;

	APlayerController* PlayerController = GetController<APlayerController>();
	if (PlayerController != nullptr)
	{
		ABattleHUD* HUD = PlayerController->GetHUD<ABattleHUD>();
		if (HUD != nullptr)
		{
			HUD->BeginDrawSelectBox();
		}
	}
}

void ACommanderPawn::EndSelect(bool bAdditional, bool bSkipSelect)
{
	bSelectPressed = false;

	APlayerController* PlayerController = GetController<APlayerController>();
	if (PlayerController != nullptr)
	{
		ABattleHUD* HUD = PlayerController->GetHUD<ABattleHUD>();
		if (HUD != nullptr)
		{
			const FBox2D SelectBox = HUD->EndDrawSelectBox();

			if (bSkipSelect)
			{
				return;
			}

			static FMultiLineTraceResult Result;
			SelectBoxLineTracePawn(PlayerController, SelectBox, Result);

			// deselect all selected pawns
			// - if we have selected other's pawn when additional select owned pawn
			// - it's not additional
			if (!bAdditional
				|| (Result.bHasOwned && !IsOwned(GetFirstSelectedPawn())))
			{
				// deselected
				DeselectAll();
			}

			if (bAdditional || Result.bHasOwned)
			{
				// only select owned pawn when using additional select or there is owned pawn in hit pawns
				for (APFPawn* Pawn : Result.HitPawns)
				{
					if (IsOwned(Pawn))
					{
						Select(Pawn);
					}
				}
			}
			else if (Result.FirstOthersPawn != nullptr)
			{
				Select(Result.FirstOthersPawn);
			}

			// DEBUG_MESSAGE(TEXT("Selected Actors [%d]"), SelectedPawns.Num());
			// for (AActor* Actor : SelectedPawns)
			// {
			// 	DEBUG_MESSAGE(TEXT("Selected Actor [%s]"), *Actor->GetName());
			// }
		}
	}
}

void ACommanderPawn::Select(APFPawn* Pawn)
{
	if (Pawn != nullptr && !Pawn->HasSelected())
	{
		SelectedPawns.Add(Pawn);
		Pawn->OnSelected(this);
	}
}

void ACommanderPawn::Deselect(APFPawn* Pawn)
{
	if (Pawn != nullptr && Pawn->HasSelected())
	{
		Pawn->OnDeselected();
		SelectedPawns.Remove(Pawn);
	}
}

void ACommanderPawn::DeselectAll()
{
	for (APFPawn* Pawn : SelectedPawns)
	{
		Pawn->OnDeselected();
	}
	SelectedPawns.Reset();
}

void ACommanderPawn::SelectBoxLineTracePawn(const APlayerController* PlayerController, const FBox2D& SelectBox,
                                            FMultiLineTraceResult& OutResult) const
{
	// DEBUG_MESSAGE(TEXT("Select Box: %s"), *SelectBox.ToString());
	
	int32 ViewportSizeX, ViewportSizeY;
	PlayerController->GetViewportSize(ViewportSizeX, ViewportSizeY);

	const float SubFactor = ViewportSizeY / SpringArm->TargetArmLength;
	const float Step = FMath::Max(1.f, SubFactor * LineTraceStepFactor);

	// DEBUG_MESSAGE(TEXT("Line Trace SubFactor: %.2f"), SubFactor);
	// DEBUG_MESSAGE(TEXT("Line Trace Factor: %s"), *LineTraceStepFactor.ToString());
	// DEBUG_MESSAGE(TEXT("Line Trace Step: %.2f"), Step);

	OutResult.HitPawns.Reset();
	OutResult.bHasOwned = false;
	OutResult.FirstOthersPawn = nullptr;

	FHitResult HitResult;
	for (float X = SelectBox.Min.X; X <= SelectBox.Max.X; X += Step)
	{
		for (float Y = SelectBox.Min.Y; Y <= SelectBox.Max.Y; Y += Step)
		{
			LineTrace(PlayerController, FVector2D(X, Y), HitResult);
			APFPawn* Pawn = Cast<APFPawn>(HitResult.Actor.Get());
			if (Pawn != nullptr)
			{
				OutResult.HitPawns.Add(Pawn);

				if (IsOwned(Pawn))
				{
					OutResult.bHasOwned = true;
				}
				else
				{
					OutResult.FirstOthersPawn = Pawn;
				}
			}
		}
	}
}

void ACommanderPawn::LineTrace(const APlayerController* Player, const FVector2D& ScreenPoint,
                               FHitResult& OutResult) const
{
	FVector LineStart, LineEnd;
	UGameplayStatics::DeprojectScreenToWorld(Player, ScreenPoint, LineStart, LineEnd);
	LineEnd = LineStart + LineEnd * LineTraceDistance;

	// static TArray<FHitResult> TempHitResults;
	GetWorld()->LineTraceSingleByChannel(OutResult, LineStart, LineEnd, ECollisionChannel::ECC_Visibility);
	// DrawDebugLine(GetWorld(), LineStart, LineEnd, FColor::Yellow, false, 5);
}

bool ACommanderPawn::IsOwned(APFPawn* Pawn) const
{
	return Pawn != nullptr && Pawn->GetOwnerPlayer() == GetPlayerState<APFPlayerState>();
}

void ACommanderPawn::Send_Implementation(const FCommandInfo& CommandInfo)
{
	for (APFPawn* Pawn : SelectedPawns)
	{
		if (!IsOwned(Pawn))
		{
			return;
		}
			
		AConsciousPawn* ConsciousPawn = Cast<AConsciousPawn>(Pawn);
		if (ConsciousPawn != nullptr)
		{
			ConsciousPawn->Receive(CommandInfo);
		}
	}
}

void ACommanderPawn::TargetPressed()
{
	if (SelectedPawns.Num() == 0)
	{
		return;
	}
	
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController != nullptr)
	{
		FVector2D MousePosition;
		PlayerController->GetMousePosition(MousePosition.X, MousePosition.Y);

		FHitResult HitResult;
		LineTrace(PlayerController, MousePosition, HitResult);

		if (!HitResult.IsValidBlockingHit())
		{
			return;
		}
		
		FCommandInfo CommandInfo;
		CommandInfo.CommandName = NAME_None;
		CommandInfo.TargetLocation = HitResult.Location;
		CommandInfo.TargetPawn = Cast<APFPawn>(HitResult.Actor.Get());

		Send(CommandInfo);
	}
}

void ACommanderPawn::TargetReleased()
{
	// keep
}

void ACommanderPawn::SpawnPawn_Implementation(TSubclassOf<APFPawn> PawnClass, FVector Location)
{
	UPFBlueprintFunctionLibrary::SpawnPawnForCommander(this, PawnClass, this, Location, FRotator(0, 0, 0));
}

void ACommanderPawn::Test_Implementation()
{
}

void ACommanderPawn::CameraScale(float Value)
{
	// UE_LOG_TEMP(TEXT("Camera Scale: %f"), Value);
	CameraScaleValue = FMath::Clamp(CameraScaleValue + Value * CameraScaleSpeed, 0.f, 1.f);
}

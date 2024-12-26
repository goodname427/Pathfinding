// Fill out your copyright notice in the Description page of Project Settings.


#include "CommanderPawn.h"

#include "BattleHUD.h"
#include "PFBlueprintFunctionLibrary.h"
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
	INIT_DEFAULT_SUBOBJECT(SpringArmComponent);
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, 50.0f), FRotator(-45.0f, 0.0f, 0.0f));
	SpringArmComponent->TargetArmLength = 3000.f;
	SpringArmComponent->bEnableCameraLag = true;
	SpringArmComponent->CameraLagSpeed = 3.0f;
	SpringArmComponent->bDoCollisionTest = false;

	// Camera
	INIT_DEFAULT_SUBOBJECT(CameraComponent);
	CameraComponent->SetupAttachment(SpringArmComponent, USpringArmComponent::SocketName);

	// Movement
	MovementComponent = CreateDefaultSubobject<UPawnMovementComponent, UCommanderPawnMovementComponent>("MovementComponent");
	MovementComponent->UpdatedComponent = RootComponent;

	// Camera Scale
	MinTargetArmLength = 1500.f;
	MaxTargetArmLength = 8000.f;
	CameraScaleSpeed = 0.1f;
	CameraScaleValue = 0.5f;

	// Mouse Move
	bEnableMouseMove = false;

	// Camera Rotate
	bUseControllerRotationYaw = true;
	CameraRotateSpeed = 200.f;
	bControlPressed = false;

	// Select
	LineTraceDistance = 200 * 100.f; // 100 m

	// Flag
	INIT_DEFAULT_SUBOBJECT(StaticMeshComponent);
	StaticMeshComponent->SetupAttachment(RootComponent);
	StaticMeshComponent->SetCollisionProfileName(FName("NoCollision"));
}

void ACommanderPawn::PostInitProperties()
{
	Super::PostInitProperties();

	SpringArmComponent->TargetArmLength = FMath::Lerp(MinTargetArmLength, MaxTargetArmLength, 0.5f);
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

void ACommanderPawn::BeginPlay()
{
	Super::BeginPlay();
}

void ACommanderPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Camera Arm Lerp
	SpringArmComponent->TargetArmLength = FMath::Lerp(SpringArmComponent->TargetArmLength,
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

	if (!bTargeting)
	{
		BeginSelect();
	}
	else
	{
		Target(TargetingCommandName);
	}
}

void ACommanderPawn::SelectReleased()
{
	if (!bTargeting)
	{
		EndSelect(false, bDoubleClick);
	}
}

void ACommanderPawn::Select_CtrlPressed()
{
	if (!bTargeting)
	{
		BeginSelect();
	}
	else
	{
		Target(TargetingCommandName);
	}
}

void ACommanderPawn::Select_CtrlReleased()
{
	if (!bTargeting)
	{
		EndSelect(true);
	}
}

void ACommanderPawn::SelectDoubleClick()
{
	if (bTargeting)
	{
		return;
	}
	
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
	if (bSelecting)
	{
		EndSelect(false, true);
	}
	
	bSelecting = true;
	
	APlayerController* PlayerController = GetController<APlayerController>();
	if (PlayerController)
	{
		PlayerController->GetMousePosition(SelectBoxBeginMousePos.X, SelectBoxBeginMousePos.Y);
	}
}

void ACommanderPawn::EndSelect(bool bAdditional, bool bSkipSelect)
{
	if (!bSelecting)
	{
		return;
	}
	
	bSelecting = false;
	
	if (bSkipSelect)
	{
		return;
	}

	const APlayerController* PlayerController = GetController<APlayerController>();
	FBox2D SelectBox;
	if (PlayerController != nullptr)
	{
		FVector2D MousePosition;
		PlayerController->GetMousePosition(MousePosition.X, MousePosition.Y);
		SelectBox = FBox2D(FVector2D::Min(SelectBoxBeginMousePos, MousePosition),
		                   FVector2D::Max(SelectBoxBeginMousePos, MousePosition));
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

void ACommanderPawn::Select(APFPawn* Pawn)
{
	if (Pawn != nullptr && !Pawn->HasSelected())
	{
		Pawn->OnSelected(this);
		ServerSelect(Pawn);
		
		if (OnSelectedPawnChanged.IsBound())
		{
			OnSelectedPawnChanged.Broadcast(this);
		}
	}
}

void ACommanderPawn::Deselect(APFPawn* Pawn)
{
	if (Pawn != nullptr && Pawn->HasSelected())
	{
		Pawn->OnDeselected();
		ServerDeselect(Pawn);

		if (OnSelectedPawnChanged.IsBound())
		{
			OnSelectedPawnChanged.Broadcast(this);
		}
	}
}

void ACommanderPawn::DeselectAll()
{
	for (APFPawn* Pawn : SelectedPawns)
	{
		Pawn->OnDeselected();
	}
	ServerDeselectAll();

	if (OnSelectedPawnChanged.IsBound())
	{
		OnSelectedPawnChanged.Broadcast(this);
	}
}

void ACommanderPawn::ServerSelect_Implementation(APFPawn* Pawn)
{
	SelectedPawns.Add(Pawn);
}

void ACommanderPawn::ServerDeselect_Implementation(APFPawn* Pawn)
{
	SelectedPawns.Remove(Pawn);
}

void ACommanderPawn::ServerDeselectAll_Implementation()
{
	SelectedPawns.Reset();
}

void ACommanderPawn::SelectBoxLineTracePawn(const APlayerController* PlayerController, const FBox2D& SelectBox,
                                            FMultiLineTraceResult& OutResult) const
{
	// DEBUG_MESSAGE(TEXT("Select Box: %s"), *SelectBox.ToString());

	int32 ViewportSizeX, ViewportSizeY;
	PlayerController->GetViewportSize(ViewportSizeX, ViewportSizeY);

	const float SubFactor = ViewportSizeY / SpringArmComponent->TargetArmLength;
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
	GetWorld()->LineTraceSingleByChannel(OutResult, LineStart, LineEnd, ECC_Camera);
	// DrawDebugLine(GetWorld(), LineStart, LineEnd, FColor::Yellow, false, 5);
}

bool ACommanderPawn::IsOwned(APFPawn* Pawn) const
{
	return Pawn != nullptr && Pawn->GetOwnerPlayer() == GetPlayerState();
}

void ACommanderPawn::Send_Implementation(const FTargetRequest& Request)
{
	// DEBUG_MESSAGE(TEXT("SelectPawns Count [%d]"), SelectedPawns.Num());

	int32 NumSent = 0;
	for (APFPawn* Pawn : SelectedPawns)
	{
		if (!IsOwned(Pawn))
		{
			return;
		}

		// if (Pawn->GetOwner() == this)
		// {
		// 	DEBUG_MESSAGE(TEXT("Owner Of Pawn [%s] Is [%s]. Me [%s]"), *Pawn->GetName(), *Pawn->GetOwner()->GetName(), *GetName());
		// }
		// FString PawnLocalRole;
		// switch (Pawn->GetLocalRole())
		// {
		// case ROLE_Authority:
		// 	PawnLocalRole = FString("ROLE_Authority");
		// 	break;
		// case ROLE_AutonomousProxy:
		// 	PawnLocalRole = FString("ROLE_AutonomousProxy");
		// 	break;
		// case ROLE_SimulatedProxy:
		// 	PawnLocalRole = FString("ROLE_SimulatedProxy");
		// 	break;
		// case ROLE_None:
		// 	PawnLocalRole = FString("ROLE_None");
		// 	break;
		// case ROLE_MAX:
		// 	PawnLocalRole = FString("ROLE_MAX");
		// 	break;
		// }
		// DEBUG_MESSAGE(TEXT("Role Of Pawn Is [%s]"), *PawnLocalRole);

		AConsciousPawn* ConsciousPawn = Cast<AConsciousPawn>(Pawn);
		if (ConsciousPawn != nullptr)
		{
			NumSent++;
			ConsciousPawn->Receive(Request);
		}
	}

	// DEBUG_MESSAGE(TEXT("SelectPawns Sent / All [%d / %d]"), NumSent, SelectedPawns.Num());
}

void ACommanderPawn::BeginTarget(FName InTargetingCommandName)
{
	bTargeting = true;
	TargetingCommandName = InTargetingCommandName;
}

void ACommanderPawn::EndTarget()
{
	bTargeting = false;
	TargetingCommandName = NAME_None;
}

void ACommanderPawn::TargetPressed()
{
	if (!bTargeting)
	{
		Target();
	}
	else
	{
		EndTarget();
	}
}

void ACommanderPawn::TargetReleased()
{
	// keep
}

void ACommanderPawn::Target(FName CommandName)
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

		FTargetRequest Request;
		Request.CommandName = CommandName;
		Request.TargetLocation = HitResult.Location;
		Request.TargetPawn = Cast<APFPawn>(HitResult.Actor.Get());

		if (Request.TargetPawn)
		{
			Request.TargetPawn->OnTarget(this);
		}

		Send(Request);
	}

	if (bTargeting)
	{
		EndTarget();
	}
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

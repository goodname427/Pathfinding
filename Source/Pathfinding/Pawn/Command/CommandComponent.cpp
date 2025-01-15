// Fill out your copyright notice in the Description page of Project Settings.


#include "CommandComponent.h"

#include "CommanderPawn.h"
#include "ConsciousPawn.h"
#include "PFUtils.h"

// #define COMMAND_DEBUG

FTargetRequest::FTargetRequest(UCommandComponent* InCommand) : FTargetRequest()
{
	Command = InCommand;

	CommandName = Command ? InCommand->GetCommandName() : NAME_None;
}

UCommandComponent::UCommandComponent(): TargetCommander(nullptr)
{
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);
	bWantsInitializeComponent = true;
}

void UCommandComponent::TickComponent(float DeltaTime, enum ELevelTick TickType,
                                      FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (IsExecuting())
	{
		InternalExecute(DeltaTime);
	}

	if (IsTargeting())
	{
		InternalTarget(DeltaTime);
	}
}

void UCommandComponent::InitializeComponent()
{
	Super::InitializeComponent();

	// DEBUG_FUNC_FLAG();
	if (AConsciousPawn* ConsciousPawn = GetExecutePawn())
	{
		ConsciousPawn->AddCommand(this);
	}
}

void UCommandComponent::UninitializeComponent()
{
	Super::UninitializeComponent();

	if (AConsciousPawn* ConsciousPawn = GetExecutePawn())
	{
		ConsciousPawn->RemoveCommand(this);
	}
}

FString UCommandComponent::GetCommandDescription_Implementation() const
{
	return Data.Description;
}

FString UCommandComponent::GetCommandDisplayName_Implementation() const
{
	return GetCommandName().ToString();
}

float UCommandComponent::GetRequiredTargetRadius_Implementation() const
{
	return Data.GetRequiredTargetRadius();
}

UObject* UCommandComponent::GetCommandIcon_Implementation() const
{
	return Data.Icon;
}

AConsciousPawn* UCommandComponent::GetExecutePawn() const
{
	return Cast<AConsciousPawn>(GetOwner());
}

UActorComponent* UCommandComponent::GetComponentFromExecutePawn(TSubclassOf<UActorComponent> ComponentClass) const
{
	if (const AActor* Actor = GetOwner())
	{
		return Actor->GetComponentByClass(ComponentClass);
	}

	return nullptr;
}

AConsciousAIController* UCommandComponent::GetExecuteController() const
{
	if (const AConsciousPawn* ExecutePawn = GetExecutePawn())
	{
		return Cast<AConsciousAIController>(ExecutePawn->GetController());
	}

	return nullptr;
}

ABattlePlayerState* UCommandComponent::GetExecutePlayerState() const
{
	if (const AConsciousPawn* ExecutePawn = GetExecutePawn())
	{
		return ExecutePawn->GetOwnerPlayer();
	}

	return nullptr;
}

ACommanderPawn* UCommandComponent::GetExecuteCommander() const
{
	if (const AConsciousPawn* ExecutePawn = GetExecutePawn())
	{
		return ExecutePawn->GetOwner<ACommanderPawn>();
	}

	return nullptr;
}

bool UCommandComponent::IsCommandEnable(bool bCheckBeforeExecute) const
{
	if (bCheckBeforeExecute && !Data.bCommandEnableCheckBeforeExecute)
	{
		return true;
	}

	return GetExecutePawn() != nullptr && InternalIsCommandEnable();
}

void UCommandComponent::SetCommandArgumentsSkipCheck(const FTargetRequest& InRequest)
{
	Request = InRequest;
}

bool UCommandComponent::SetCommandArguments(const FTargetRequest& InRequest)
{
	SetCommandArgumentsSkipCheck(InRequest);

	return IsArgumentsValid();
}

bool UCommandComponent::IsArgumentsValid(bool bCheckBeforeExecute) const
{
	if (bCheckBeforeExecute && !Data.bArgumentsValidCheckBeforeExecute)
	{
		return true;
	}

	return !IsNeedToTarget() || InternalIsArgumentsValid();
}

bool UCommandComponent::IsTargetInRequiredRadius() const
{
	const float TargetRadius = GetRequiredTargetRadius();
	if (TargetRadius < 0)
	{
		return true;
	}

	const AConsciousPawn* ExecutePawn = GetExecutePawn();

	if (!Request.IsTargetPawnValid())
	{
		return FVector::Dist(Request.TargetLocation, ExecutePawn->GetActorLocation()) <= TargetRadius;
	}

	const FVector CurrentLocation = ExecutePawn->GetActorLocation();

	TArray<FHitResult> HitResults;
	// FCollisionQueryParams Params;
	// Params.AddIgnoredActor(ExecutePawn);

	GetWorld()->SweepMultiByChannel(
		HitResults,
		CurrentLocation,
		CurrentLocation,
		FQuat::Identity,
		ECC_Camera,
		FCollisionShape::MakeSphere(TargetRadius),
		FCollisionQueryParams::DefaultQueryParam
	);

	for (const FHitResult& Hit : HitResults)
	{
		if (Hit.Actor == Request.TargetPawn)
		{
			return true;
		}
	}

	return false;
}

void UCommandComponent::InternalBeginExecute_Implementation()
{
}

bool UCommandComponent::InternalCanExecute_Implementation() const
{
	return true;
}

bool UCommandComponent::CanExecute() const
{
	return IsCommandEnable(true)
		&& IsArgumentsValid(true)
		&& IsTargetInRequiredRadius()
		&& InternalCanExecute();
}

void UCommandComponent::BeginExecute()
{
	if (bExecuting)
	{
		return;
	}

	if (IsTargeting())
	{
		EndTarget(true);
	}

	bExecuting = true;

	if (OnCommandBegin.IsBound())
	{
		OnCommandBegin.Broadcast(this);
	}

#ifdef COMMAND_DEBUG
	DEBUG_MESSAGE(
		TEXT("Conscious Pawn [%s] Execute Command [%s]"),
		*GetExecutePawn()->GetName(),
		*GetCommandName().ToString()
	);
#endif

	InternalBeginExecute();
}

void UCommandComponent::EndExecute(ECommandExecuteResult Result)
{
	if (!bExecuting)
	{
		return;
	}

	bExecuting = false;

#ifdef COMMAND_DEBUG
	DEBUG_MESSAGE(
		TEXT("Conscious Pawn [%s] End Execute Command [%s]"),
		*GetExecutePawn()->GetName(),
		*GetCommandName().ToString()
	);
#endif

	InternalEndExecute(Result);

	if (OnCommandEnd.IsBound())
	{
		OnCommandEnd.Broadcast(this, Result);
	}
}

void UCommandComponent::EndExecuteDelay(ECommandExecuteResult Result, float Duration)
{
	auto L = [this, Result]() { EndExecute(Result); };

	Delay(this, Duration, L);
	// UDelayHelper::Delay(this, Duration, L);
	// TSharedPtr<FTimerHandle> TimerHandle = MakeShared<FTimerHandle>();
	//
	// GetWorld()->GetTimerManager().SetTimer(
	// 	*TimerHandle,
	// 	FTimerDelegate::CreateLambda(),
	// 	Duration,
	// 	false
	// );
}

void UCommandComponent::BeginTarget(ACommanderPawn* InTargetCommander)
{
	if (bTargeting || InTargetCommander == nullptr)
	{
		return;
	}

	bTargeting = true;
	TargetCommander = InTargetCommander;

	InternalBeginTarget();
}

void UCommandComponent::EndTarget(bool bCanceled)
{
	if (!bTargeting)
	{
		return;
	}

	bTargeting = false;

	InternalEndTarget(bCanceled);

	TargetCommander = nullptr;
}

void UCommandComponent::OnPushedToQueue()
{
	if (OnCommandPushedToQueue.IsBound())
	{
		OnCommandPushedToQueue.Broadcast(this);
	}

#ifdef COMMAND_DEBUG
	DEBUG_MESSAGE(
		TEXT("Conscious Pawn [%s] Push Command [%s]"),
		*GetExecutePawn()->GetName(),
		*GetCommandName().ToString()
	);
#endif

	InternalPushedToQueue();
}

void UCommandComponent::OnPoppedFromQueue(ECommandPoppedReason Reason)
{
#ifdef COMMAND_DEBUG
	DEBUG_MESSAGE(
		TEXT("Conscious Pawn [%s] Pop Command [%s]"),
		*GetExecutePawn()->GetName(),
		*GetCommandName().ToString()
	);
#endif

	InternalPoppedFromQueue(Reason);

	if (OnCommandPoppedFromQueue.IsBound())
	{
		OnCommandPoppedFromQueue.Broadcast(this, Reason);
	}
}

void UCommandComponent::InternalTarget_Implementation(float DeltaTime)
{
}

void UCommandComponent::InternalEndTarget_Implementation(bool bCanceled)
{
}

void UCommandComponent::InternalBeginTarget_Implementation()
{
}

bool UCommandComponent::InternalIsCommandEnable_Implementation() const
{
	return true;
}

void UCommandComponent::InternalExecute_Implementation(float DeltaTime)
{
}

void UCommandComponent::InternalPoppedFromQueue_Implementation(ECommandPoppedReason Reason)
{
}

void UCommandComponent::InternalPushedToQueue_Implementation()
{
}

bool UCommandComponent::InternalIsArgumentsValid_Implementation() const
{
	return true;
}

void UCommandComponent::InternalEndExecute_Implementation(ECommandExecuteResult Result)
{
}

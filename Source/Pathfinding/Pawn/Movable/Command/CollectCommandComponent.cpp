// Fill out your copyright notice in the Description page of Project Settings.


#include "CollectCommandComponent.h"

#include "CommanderPawn.h"
#include "PFPawn.h"
#include "PFUtils.h"
#include "TransportCommandComponent.h"
#include "Building/BaseCampPawn.h"
#include "Resource/ResourcePawn.h"

FName UCollectCommandComponent::StaticCommandName = FName("Collect");

UCollectCommandComponent::UCollectCommandComponent(): ResourceTypesToAllowCollecting({EResourceType::Coin})
{
	PrimaryComponentTick.bCanEverTick = true;

	Data.Name = StaticCommandName;
	Data.bHiddenInCommandListMenu = true;
}

bool UCollectCommandComponent::CanCollect(const AResourcePawn* ResourcePawn) const
{
	if (ResourcePawn == nullptr || ResourcePawn->IsPendingKill() || ResourcePawn->GetResourcePoint() <= 0)
	{
		return false;
	}

	return ResourceTypesToAllowCollecting.Contains(ResourcePawn->GetResourceType());
}

bool UCollectCommandComponent::InternalIsCommandEnable_Implementation() const
{
	return GetComponentFromExecutePawn<UCollectorComponent>() != nullptr && GetExecutePlayerState() != nullptr;
}

bool UCollectCommandComponent::InternalIsArgumentsValid_Implementation() const
{
	if (const AResourcePawn* ResourcePawn = Request.GetTargetPawn<AResourcePawn>())
	{
		if (CanCollect(ResourcePawn))
		{
			// DEBUG_MESSAGE(
			// 	TEXT("Collector [%s] Can Collect Resource [%s] [%d]"),
			// 	*GetExecutePawn()->GetName(),
			// 	*ResourcePawn->GetName(),
			// 	ResourcePawn->GetResourcePoint()
			// );
			return true;
		}
	}

	return false;
}

void UCollectCommandComponent::InternalBeginExecute_Implementation()
{
	AUTHORITY_CHECK();

	AResourcePawn* Resource = Request.GetTargetPawn<AResourcePawn>();
	const AConsciousPawn* ExecutePawn = GetExecutePawn();
	UCollectorComponent* Collector = GetComponentFromExecutePawn<UCollectorComponent>();

	if (Resource && ExecutePawn && Collector && Resource->CollectBy(Collector))
	{
		EndExecuteDelay(ECommandExecuteResult::Success, 1 / ExecutePawn->GetAttackDuration());
	}
	else
	{
		EndExecuteDelay(ECommandExecuteResult::Failed, 1 / ExecutePawn->GetAttackDuration());
	}
}

void UCollectCommandComponent::InternalEndExecute_Implementation(ECommandExecuteResult Result)
{
	GetExecutePawn()->SetActorScale3D(FVector(1));
}

void UCollectCommandComponent::InternalExecute_Implementation(float DeltaTime)
{
	AConsciousPawn* Pawn = GetExecutePawn();
	Pawn->SetActorScale3D(FVector(Pawn->GetActorScale3D().X + DeltaTime));
}

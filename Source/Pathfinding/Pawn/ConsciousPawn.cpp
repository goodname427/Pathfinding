// Fill out your copyright notice in the Description page of Project Settings.


#include "ConsciousPawn.h"

#include "Controller/ConsciousAIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Component/CommandChannelComponent.h"
#include "Command/MoveCommandComponent.h"
#include "Component/AttackNotifierComponent.h"

// Sets default values
AConsciousPawn::AConsciousPawn(): ConsciousData()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AIControllerClass = AConsciousAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	INIT_DEFAULT_SUBOBJECT(AttackNotifierComponent);
}

void AConsciousPawn::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void AConsciousPawn::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority() && ConsciousData.FoodCostPerCycle <= 0)
	{
		if (const UWorld* World = GetWorld())
		{
			const UPFGameSettings* GameSettings = GetDefault<UPFGameSettings>();

			World->GetTimerManager().SetTimer(
				CostFoodTimer,
				FTimerDelegate::CreateUObject(this, &AConsciousPawn::CostFood),
				GameSettings ? GameSettings->FoodCostCycleDuration : 10,
				true
			);
		}
	}
}

void AConsciousPawn::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (const UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(CostFoodTimer);
	}
}

void AConsciousPawn::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// RefreshCommandList();
}

void AConsciousPawn::Receive_Implementation(const FTargetRequest& Request)
{
	// DEBUG_MESSAGE(TEXT("Pawn [%s] Received Request [%s]"), *GetName(), *Request.CommandName.ToString());

	OnReceive(Request);
	if (OnReceivedRequest.IsBound())
	{
		OnReceivedRequest.Broadcast(this, Request);
	}

	if (Request.Type == ETargetRequestType::AbortOrPop
		|| Request.Type == ETargetRequestType::Clear)
	{
		UCommandChannelComponent* RequestCommandChannel = GetCommandChannel(Request.OverrideCommandChannel);

		if (!RequestCommandChannel)
		{
			return;
		}

		switch (Request.Type)
		{
		case ETargetRequestType::AbortOrPop:
			{
				RequestCommandChannel->PopCommand(Request.CommandIndexToPop);
				break;
			}
		case ETargetRequestType::Clear:
			{
				CLEAR_COMMAND_CHANNEL(RequestCommandChannel, ECommandPoppedReason::Cancel);
				break;
			}
		default:
			{
				break;
			}
		}

		return;
	}

	// Append and StartNew
	static TArray<UCommandComponent*> CommandsToExecute;
	ResolveRequest(CommandsToExecute, Request);

	if (CommandsToExecute.Num() == 0)
	{
		return;
	}

	const UCommandComponent* FirstCommand = CommandsToExecute[0];
	UCommandChannelComponent* CommandChannel = GetOrCreateCommandChannel(GET_COMMAND_CHANNEL(Request, FirstCommand));

	const bool bAbortCurrentCommand = Request.Type == ETargetRequestType::StartNew && FirstCommand->
		IsAbortCurrentCommand();

	if (bAbortCurrentCommand)
	{
		CLEAR_COMMAND_CHANNEL(CommandChannel, ECommandPoppedReason::Cancel);
	}

	for (UCommandComponent* CommandToExecute : CommandsToExecute)
	{
		VALID_CHECK(CommandToExecute);

		// DEBUG_MESSAGE(TEXT("ConsciousAIController Push Command [%s]"), *CommandToExecute->GetCommandName().ToString());
		CommandChannel->PushCommand(CommandToExecute);
	}

	// if (bAbortCurrentCommand || CommandChannel->GetCurrentCommand() == nullptr)
	// {
	// 	CommandChannel->ExecuteNextCommand();
	// }
}

void AConsciousPawn::OnReceive_Implementation(const FTargetRequest& Request)
{
}

void AConsciousPawn::ResolveRequest(TArray<UCommandComponent*>& OutCommandsToExecute, const FTargetRequest& Request)
{
	OutCommandsToExecute.Reset();
	UCommandComponent* RequestCommand = ResolveRequestCommand(Request);

	// no command
	if (RequestCommand == nullptr || !RequestCommand->IsCommandEnable())
	{
		return;
	}

	UMoveCommandComponent* MoveCommandComponent = GetMoveCommandComponent();

	if (RequestCommand == MoveCommandComponent)
	{
		if (MoveCommandComponent->SetMoveCommandArguments(nullptr, Request))
		{
			OutCommandsToExecute.Add(MoveCommandComponent);;
		}
	}
	else
	{
		if (RequestCommand->SetCommandArguments(Request))
		{
			// needn't to move
			if (RequestCommand->IsTargetInRequiredRadius())
			{
				OutCommandsToExecute.Add(RequestCommand);
			}
			// add move command if the request command need to move
			else if (MoveCommandComponent != nullptr)
			{
				if (MoveCommandComponent->SetMoveCommandArguments(RequestCommand, Request))
				{
					OutCommandsToExecute.Add(MoveCommandComponent);
				}

				OutCommandsToExecute.Add(RequestCommand);
			}
		}
	}
}

UCommandComponent* AConsciousPawn::ResolveRequestCommand(const FTargetRequest& Request)
{
	UCommandComponent* RequestCommand = nullptr;
	if (Request.Command != nullptr && Request.Command->GetExecutePawn() == this)
	{
		RequestCommand = Request.Command;
	}
	else if (Request.CommandName != NAME_None)
	{
		if (UCommandComponent** RequestCommandPtr = CommandList.Find(Request.CommandName))
		{
			RequestCommand = *RequestCommandPtr;
		}
	}
	else
	{
		RequestCommand = ResolveRequestWithoutName(Request);
	}

	return RequestCommand;
}

UMoveCommandComponent* AConsciousPawn::GetMoveCommandComponent() const
{
	return GetCommandByName<UMoveCommandComponent>();
}

UCommandComponent* AConsciousPawn::GetCommandByName(FName CommandName) const
{
	if (UCommandComponent* const* FoundCommand = CommandList.Find(CommandName))
	{
		return *FoundCommand;
	}
	return nullptr;
}

const TArray<UCommandComponent*>& AConsciousPawn::GetCommandsByName(FName CommandName) const
{
	static TArray<UCommandComponent*> FoundCommands;
	FoundCommands.Reset();

	CommandList.MultiFind(CommandName, FoundCommands);

	return FoundCommands;
}

const TArray<UCommandComponent*>& AConsciousPawn::GetAllCommands() const
{
	static TArray<UCommandComponent*> CommandArray;

	CommandList.GenerateValueArray(CommandArray);

	return CommandArray;
}

const TArray<UCommandComponent*>& AConsciousPawn::GetCommandsByClass(
	TSubclassOf<UCommandComponent> CommandClass) const
{
	static TArray<UCommandComponent*> CommandArray;
	CommandList.GenerateValueArray(CommandArray);

	static TArray<UCommandComponent*> CommandArrayOfSpecifyClass;
	CommandArrayOfSpecifyClass.Reset();

	for (UCommandComponent* Command : CommandArray)
	{
		if (Command->GetClass() == CommandClass)
		{
			CommandArrayOfSpecifyClass.Add(Command);
		}
	}

	return CommandArray;
}

const TArray<UCommandComponent*>& AConsciousPawn::GetAllCommandsForCommandListMenu() const
{
	static TArray<UCommandComponent*> CommandArray;

	CommandList.GenerateValueArray(CommandArray);

	// Remove all hidden commands
	for (int32 i = 0; i < CommandArray.Num();)
	{
		if (CommandArray[i]->IsHideInCommandListMenu())
		{
			CommandArray.RemoveAt(i);
			continue;
		}
		i++;
	}

	CommandArray.SetNum(16);

	// reorder by wants index
	for (int32 i = 0; i < CommandArray.Num(); i++)
	{
		if (CommandArray[i] == nullptr)
		{
			continue;
		}

		const int32 WantsIndex = CommandArray[i]->GetWantsIndexInCommandListMenu();
		if (WantsIndex != i && WantsIndex >= 0 && WantsIndex < CommandArray.Num())
		{
			Swap(CommandArray[i], CommandArray[WantsIndex]);
		}
	}

	return CommandArray;
}

void AConsciousPawn::RefreshCommandList()
{
	CommandList.Empty();
	for (UActorComponent* Component : GetComponents())
	{
		UCommandComponent* Command = Cast<UCommandComponent>(Component);
		if (Command)
		{
			CommandList.Add(Command->GetCommandName(), Command);
		}
	}

	if (OnCommandListUpdated.IsBound())
	{
		OnCommandListUpdated.Broadcast(this);
	}
}

UCommandComponent* AConsciousPawn::AddNewCommand(TSubclassOf<UCommandComponent> CommandClassToAdd)
{
	UCommandComponent* NewCommand = Cast<UCommandComponent>(
		AddComponentByClass(CommandClassToAdd, false, FTransform::Identity, false));

	return NewCommand;
}

void AConsciousPawn::AddCommand(UCommandComponent* CommandToAdd)
{
	if (CommandToAdd && CommandToAdd->GetOwner() == this)
	{
		CommandList.Add(CommandToAdd->GetCommandName(), CommandToAdd);

		if (OnCommandListUpdated.IsBound())
		{
			OnCommandListUpdated.Broadcast(this);
		}
	}
}

void AConsciousPawn::RemoveCommand(UCommandComponent* CommandToRemove)
{
	if (CommandToRemove && CommandToRemove->GetOwner() == this)
	{
		CommandList.Remove(CommandToRemove->GetCommandName(), CommandToRemove);

		if (OnCommandListUpdated.IsBound())
		{
			OnCommandListUpdated.Broadcast(this);
		}
	}
}

void AConsciousPawn::DispatchCommand_OnPushedToQueue_Implementation(UCommandComponent* Command,
                                                                    const FTargetRequest& Request)
{
	if (!Command)
		return;

	if (!HasAuthority())
	{
		Command->SetCommandArgumentsSkipCheck(Request);
	}

	Command->OnPushedToQueue();
}

void AConsciousPawn::DispatchCommand_EndExecute_Implementation(UCommandComponent* Command, ECommandExecuteResult Result)
{
	if (!Command)
		return;

	Command->EndExecute(Result);
}

void AConsciousPawn::DispatchCommand_BeginExecute_Implementation(UCommandComponent* Command,
                                                                 const FTargetRequest& Request)
{
	if (!Command)
	{
		// DEBUG_MESSAGE(TEXT("Command is null"));	
		return;
	}

	if (!HasAuthority())
	{
		Command->SetCommandArgumentsSkipCheck(Request);
	}

	Command->BeginExecute();
}

void AConsciousPawn::DispatchCommand_OnPoppedFromQueue_Implementation(UCommandComponent* Command,
                                                                      ECommandPoppedReason Reason)
{
	if (!Command)
		return;

	Command->OnPoppedFromQueue(Reason);
}

UCommandComponent* AConsciousPawn::GetCurrentCommand(int32 ChannelId) const
{
	if (UCommandChannelComponent* CommandChannel = GetCommandChannel(ChannelId))
	{
		return CommandChannel->GetCurrentCommand();
	}

	return nullptr;
}

const TArray<UCommandComponent*>& AConsciousPawn::GetCommandsInQueue(int32 ChannelId) const
{
	if (UCommandChannelComponent* CommandChannel = GetCommandChannel(ChannelId))
	{
		return CommandChannel->GetCommandsInQueue();
	}

	static TArray<UCommandComponent*> EmptyCommands;
	return EmptyCommands;
}

const TArray<UProgressCommandComponent*>& AConsciousPawn::GetProgressCommandsInQueue() const
{
	static TArray<UProgressCommandComponent*> ProgressCommands;
	ProgressCommands.Reset();

	for (UCommandComponent* Command : GetCommandsInQueue(UProgressCommandComponent::StaticCommandChannel))
	{
		if (UProgressCommandComponent* ProgressCommand = Cast<UProgressCommandComponent>(Command))
		{
			ProgressCommands.Add(ProgressCommand);
		}
	}

	return ProgressCommands;
}

UCommandChannelComponent* AConsciousPawn::GetCommandChannel(int32 ChannelId) const
{
	if (ChannelId < GCommandChannel_Default)
	{
		return nullptr;
	}

	if (UCommandChannelComponent* const* CommandChannel = CommandChannelMap.Find(ChannelId))
	{
		return *CommandChannel;
	}

	return nullptr;
}

UCommandChannelComponent* AConsciousPawn::GetOrCreateCommandChannel(int32 ChannelId)
{
	if (GetLocalRole() < ROLE_Authority)
	{
		return nullptr;
	}

	if (ChannelId < GCommandChannel_Default)
	{
		return nullptr;
	}

	if (UCommandChannelComponent*& FoundCommandChannel = CommandChannelMap.FindOrAdd(ChannelId))
	{
		return FoundCommandChannel;
	}
	else
	{
		FoundCommandChannel = UCommandChannelComponent::NewCommandChannel(this, ChannelId);
		// DEBUG_MESSAGE(TEXT("ConsciousPawn [%s] add command channel [%d]"), *GetName(), FoundCommandChannel->GetChannelId());

		if (OnCommandChannelCreated.IsBound())
		{
			OnCommandChannelCreated.Broadcast(this);
		}

		return FoundCommandChannel;
	}
}

void AConsciousPawn::AddCommandChannel(UCommandChannelComponent* CommandChannel)
{
	if (GetLocalRole() >= ROLE_Authority)
	{
		return;
	}

	if (!CommandChannelMap.Contains(CommandChannel->GetChannelId()))
	{
		// DEBUG_MESSAGE(TEXT("ConsciousPawn [%s] add command channel [%d]"), *GetName(), CommandChannel->GetChannelId());
		CommandChannelMap.Add(CommandChannel->GetChannelId(), CommandChannel);
		if (OnCommandChannelCreated.IsBound())
		{
			OnCommandChannelCreated.Broadcast(this);
		}
	}
}

void AConsciousPawn::OnDied()
{
	// Clear all command
	FTargetRequest ClearRequest;
	ClearRequest.Type = ETargetRequestType::Clear;
	ClearRequest.OverrideCommandChannel = UProgressCommandComponent::StaticCommandChannel;
	Receive(ClearRequest);
}

void AConsciousPawn::CostFood()
{
	// VALID_CHECK(this);

	if (ABattlePlayerState* PS = GetOwnerPlayer())
	{
		// VALID_CHECK(PS);

		const FResourceInfo FoodCost(EResourceType::Food, ConsciousData.FoodCostPerCycle);
		if (PS->IsResourceEnough(FoodCost))
		{
			PS->TakeResource(this, EResourceTookReason::FoodCostCycle, FoodCost);
		}
		else
		{
			const UPFGameSettings* GameSettings = GetDefault<UPFGameSettings>();

			UGameplayStatics::ApplyDamage(
				this,
				GameSettings ? GameSettings->HungerDamage : 10.0f,
				GetController(),
				this,
				UDamageType::StaticClass()
			);
		}
	}
}

UCommandComponent* AConsciousPawn::ResolveRequestWithoutName_Implementation(const FTargetRequest& Request)
{
	return nullptr;
}

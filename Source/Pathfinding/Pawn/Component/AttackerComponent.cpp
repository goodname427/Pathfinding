// Fill out your copyright notice in the Description page of Project Settings.


#include "AttackerComponent.h"

#include "ConsciousPawn.h"
#include "PFBlueprintFunctionLibrary.h"
#include "PFUtils.h"
#include "CookOnTheSide/CookOnTheFlyServer.h"


// Sets default values for this component's properties
UAttackerComponent::UAttackerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	AttackCommandComponent = nullptr;
	Attacker = nullptr;

	// ...
}

void UAttackerComponent::BeginPlay()
{
	Super::BeginPlay();
	if (Attacker == nullptr)
	{
		Attacker = GetOwner<AConsciousPawn>();	
	}
	
	if (AttackCommandComponent == nullptr)
	{
		if (Attacker)
		{
			AttackCommandComponent = Cast<UAttackCommandComponent>(Attacker->GetComponentByClass(UAttackCommandComponent::StaticClass()));
		}
	}

	if (AttackCommandComponent == nullptr || Attacker == nullptr)
	{
		// DestroyComponent();
		return;
	}

	if (GetOwnerRole() == ROLE_Authority)
	{
		AttackCommandComponent->OnCommandEnd.AddDynamic(this, &ThisClass::OnAttackCommandEnd);
		AttackCommandComponent->OnCommandPoppedFromQueue.AddDynamic(this, &ThisClass::OnAttackCommandPoppedFromQueue);

		Attacker->OnTakeAnyDamage.AddDynamic(this, &ThisClass::OnTakeAnyDamage);
	}
}

void UAttackerComponent::TickComponent(float DeltaTime, enum ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);


	
}

void UAttackerComponent::OnAttackCommandEnd(UCommandComponent* CommandComponent, ECommandExecuteResult Result)
{
	if (Result == ECommandExecuteResult::Aborted)
	{
		return;
	}

	const FTargetRequest& Request = CommandComponent->GetRequest();

	// Keep attacking if target is valid
	if (Request.IsTargetPawnValid())
	{
		Attacker->Receive(FTargetRequest(AttackCommandComponent, Request.TargetPawn));
	}
	else
	{
		APFPawn* AroundEnemyPawns = UPFBlueprintFunctionLibrary::GetAroundPawn<APFPawn>(Attacker, 1000, EPawnRole::Enemy);
		if (AroundEnemyPawns)
		{
			Attacker->Receive(FTargetRequest(AttackCommandComponent, AroundEnemyPawns));
		}
	}
}

void UAttackerComponent::OnAttackCommandPoppedFromQueue(UCommandComponent* CommandComponent,
	ECommandPoppedReason Reason)
{
	
}

void UAttackerComponent::OnTakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
	AController* InstigatedBy, AActor* DamageCauser)
{
	// Skip if damage causer is not a pawn
	APFPawn* CauserPawn = Cast<APFPawn>(DamageCauser);
	NULL_CHECK(CauserPawn);

	const EPawnRole CauserRole = Attacker->GetPawnRole(CauserPawn);
	
	// Hit back when receive damage from enemy
	if (CauserRole == EPawnRole::Enemy)
	{
		if (Attacker->GetCurrentCommand(GCommandChannel_Default) != AttackCommandComponent)
		{
			Attacker->Receive(FTargetRequest(AttackCommandComponent, CauserPawn));
		}

		// Notify other pawns around self
		// static TArray<AConsciousPawn*> AroundSelfPawns;
		// UPFBlueprintFunctionLibrary::GetAroundPawns<AConsciousPawn>(Attacker, AroundSelfPawns, 1000, EPawnRole::Self);
		// for (AConsciousPawn* Pawn : AroundSelfPawns)
		// {
		// 	const UCommandComponent* PawnCurrentCommand = Pawn->GetCurrentCommand(GCommandChannel_Default);
		// 	if (PawnCurrentCommand == nullptr || PawnCurrentCommand->GetCommandName() != UAttackCommandComponent::StaticCommandName)
		// 	{
		// 		Pawn->Receive(FTargetRequest::Make<UAttackCommandComponent>(CauserPawn));
		// 	}
		// }
	}
}

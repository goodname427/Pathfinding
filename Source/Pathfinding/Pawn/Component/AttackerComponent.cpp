// Fill out your copyright notice in the Description page of Project Settings.


#include "AttackerComponent.h"

#include "ConsciousPawn.h"


// Sets default values for this component's properties
UAttackerComponent::UAttackerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	AttackCommandComponent = nullptr;

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
		DestroyComponent();
		return;
	}

	if (GetOwnerRole() == ROLE_Authority)
	{
		AttackCommandComponent->OnCommandEnd.AddDynamic(this, &ThisClass::OnAttackCommandEnd);
		AttackCommandComponent->OnCommandPoppedFromQueue.AddDynamic(this, &ThisClass::OnAttackCommandPoppedFromQueue);
	}
}

void UAttackerComponent::OnAttackCommandEnd(UCommandComponent* CommandComponent, ECommandExecuteResult Result)
{
	if (Result == ECommandExecuteResult::Aborted)
	{
		return;
	}

	const FTargetRequest& Request = CommandComponent->GetRequest();

	if (Request.IsTargetPawnValid())
	{
		Attacker->Receive(FTargetRequest(AttackCommandComponent, Request.TargetPawn));
	}
}

void UAttackerComponent::OnAttackCommandPoppedFromQueue(UCommandComponent* CommandComponent,
	ECommandPoppedReason Reason)
{
	
}

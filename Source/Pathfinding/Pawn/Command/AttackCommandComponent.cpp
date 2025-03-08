// Fill out your copyright notice in the Description page of Project Settings.


#include "AttackCommandComponent.h"

#include "ConsciousPawn.h"
#include "Kismet/GameplayStatics.h"


FName UAttackCommandComponent::StaticCommandName = FName("Attack");

UAttackCommandComponent::UAttackCommandComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	Data.Name = StaticCommandName;
	Data.WantsIndexInCommandListMenu = 1;
}

bool UAttackCommandComponent::InternalIsCommandEnable_Implementation() const
{
	return GetExecutePawn()->CanAttack();
}

bool UAttackCommandComponent::InternalIsArgumentsValid_Implementation() const
{
	return Request.IsTargetPawnValid();
}

void UAttackCommandComponent::InternalBeginExecute_Implementation()
{
	ApplyDamageToTargetPawn();
	EndExecuteDelay(ECommandExecuteResult::Success, GetExecutePawn()->GetAttackDuration());
}

void UAttackCommandComponent::InternalEndExecute_Implementation(ECommandExecuteResult Result)
{
	GetExecutePawn()->SetActorScale3D(FVector(1));
}

void UAttackCommandComponent::InternalExecute_Implementation(float DeltaTime)
{
	AConsciousPawn* Pawn = GetExecutePawn();
	Pawn->SetActorScale3D(FVector(Pawn->GetActorScale3D().X + DeltaTime));
}

void UAttackCommandComponent::ApplyDamageToTargetPawn_Implementation()
{
	AConsciousPawn* ExecutePawn = GetExecutePawn();
	UGameplayStatics::ApplyDamage(
		Request.TargetPawn,
		ExecutePawn->GetAttack(),
		ExecutePawn->GetController(),
		ExecutePawn,
		UDamageType::StaticClass()
	);
}

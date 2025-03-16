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

	bActiveAttack = false;
	HuntingRadiusScale = 1.2f;

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
			AttackCommandComponent = Cast<UAttackCommandComponent>(
				Attacker->GetComponentByClass(UAttackCommandComponent::StaticClass()));
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

		Attacker->OnTakeAnyDamage.AddDynamic(this, &ThisClass::OnTakeAnyDamage);
	}
}

void UAttackerComponent::TickComponent(float DeltaTime, enum ELevelTick TickType,
                                       FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bActiveAttack && Attacker->GetCurrentCommand(GCommandChannel_Default) != AttackCommandComponent)
	{
		// DEBUG_MESSAGE(TEXT("Attacker [%s] Active Attack"), *Attacker->GetName());
		APFPawn* AroundEnemyPawn = UPFBlueprintFunctionLibrary::GetAroundPawn<APFPawn>(
			Attacker, AttackCommandComponent->GetRequiredTargetRadius() * HuntingRadiusScale, EPawnRole::Enemy);
		if (AroundEnemyPawn)
		{
			Attacker->Receive(FTargetRequest(AttackCommandComponent, AroundEnemyPawn));
		}
	}
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
		APFPawn* AroundEnemyPawn = UPFBlueprintFunctionLibrary::GetAroundPawn<APFPawn>(
			Attacker, AttackCommandComponent->GetRequiredTargetRadius() * HuntingRadiusScale, EPawnRole::Enemy);
		if (AroundEnemyPawn)
		{
			Attacker->Receive(FTargetRequest(AttackCommandComponent, AroundEnemyPawn));
		}
	}
}

void UAttackerComponent::OnTakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
                                         AController* InstigatedBy, AActor* DamageCauser)
{
	// Skip if damage causer is not a pawn
	APFPawn* CauserPawn = InstigatedBy ? Cast<APFPawn>(InstigatedBy->GetPawn()) : nullptr;
	NULL_CHECK(CauserPawn);

	const EPawnRole CauserRole = Attacker->GetPawnRole(CauserPawn);

	// Hit back when receive damage from enemy
	if (CauserRole == EPawnRole::Enemy)
	{
		if (Attacker->GetCurrentCommand(GCommandChannel_Default) != AttackCommandComponent)
		{
			Attacker->Receive(FTargetRequest(AttackCommandComponent, CauserPawn));
		}
	}
}

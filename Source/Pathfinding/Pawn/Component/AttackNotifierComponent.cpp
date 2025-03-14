// Fill out your copyright notice in the Description page of Project Settings.


#include "AttackNotifierComponent.h"

#include "PFBlueprintFunctionLibrary.h"
#include "PFPawn.h"
#include "PFUtils.h"
#include "Command/AttackCommandComponent.h"


// Sets default values for this component's properties
UAttackNotifierComponent::UAttackNotifierComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	OwnerPawn = nullptr;
	NotifyRadius = 1000;
}


// Called when the game starts
void UAttackNotifierComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerPawn = GetOwner<AConsciousPawn>();

	if (OwnerPawn == nullptr)
	{
		return;
	}

	if (GetOwnerRole() == ROLE_Authority)
	{
		OwnerPawn->OnTakeAnyDamage.AddDynamic(this, &ThisClass::OnTakeAnyDamage);
	}
}

void UAttackNotifierComponent::OnTakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
	AController* InstigatedBy, AActor* DamageCauser)
{
	// Skip if damage causer is not a pawn
	APFPawn* CauserPawn = InstigatedBy ? Cast<APFPawn>(InstigatedBy->GetPawn()) : nullptr;
	// DEBUG_MESSAGE(TEXT("[%s] Take Damage from [%s]"), *OwnerPawn->GetName(), *GetNameSafe(CauserPawn));
	NULL_CHECK(CauserPawn);

	const EPawnRole CauserRole = OwnerPawn->GetPawnRole(CauserPawn);
	
	if (CauserRole == EPawnRole::Enemy)
	{
		// Notify other pawns around self
		static TArray<AConsciousPawn*> AroundPawns;
		UPFBlueprintFunctionLibrary::GetAroundPawns<AConsciousPawn>(OwnerPawn, AroundPawns, NotifyRadius, EPawnRole::Self);
		for (AConsciousPawn* AroundPawn : AroundPawns)
		{
			const UCommandComponent* PawnCurrentCommand = AroundPawn->GetCurrentCommand(GCommandChannel_Default);
			// Just notify if the pawn is not attacking
			if (PawnCurrentCommand == nullptr || PawnCurrentCommand->GetCommandName() != UAttackCommandComponent::StaticCommandName)
			{
				AroundPawn->Receive(FTargetRequest::Make<UAttackCommandComponent>(CauserPawn));
			}
		}
	}
}


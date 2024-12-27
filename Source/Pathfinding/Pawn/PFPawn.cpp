// Fill out your copyright notice in the Description page of Project Settings.

#include "PFPawn.h"

#include "CommanderPawn.h"
#include "PFBlueprintFunctionLibrary.h"
#include "PFUtils.h"
#include "WidgetSettings.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

FName APFPawn::StateWidgetClassName = FName("PFPawnStateBar");

// Sets default values
APFPawn::APFPawn()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	RootComponent = INIT_DEFAULT_SUBOBJECT(StaticMeshComponent);

	// Collision
	StaticMeshComponent->SetCollisionProfileName(UCollisionProfile::Pawn_ProfileName);
	bAdjustLocationToGround = true;
	LocationToGroundOffset = 0;

	// State
	INIT_DEFAULT_SUBOBJECT(StateWidgetComponent);
	StateWidgetComponent->SetRelativeLocation(FVector::ZeroVector);
	if (const TSubclassOf<UUserWidget>* WidgetClassPtr = GetDefault<UWidgetSettings>()->WidgetClasses.Find(
		StateWidgetClassName))
	{
		StateWidgetComponent->SetWidgetClass(*WidgetClassPtr);
	}

	MaxHealth = 100;
	Attack = 1;
	AttackSpeed = 1;
	Defense = 1;
}

void APFPawn::PostInitProperties()
{
	Super::PostInitProperties();

	CurrentHealth = MaxHealth;
}

void APFPawn::BeginPlay()
{
	Super::BeginPlay();

	// Set Flag Material
	if (UMaterialInterface* FlagMaterial = Cast<UMaterialInterface>(
		GetDefault<UPFGameSettings>()->PawnFlagMaterial.TryLoad()))
	{
		UPFBlueprintFunctionLibrary::CreateDynamicMaterialInstanceForStaticMesh(StaticMeshComponent, FlagMaterial, 0);
		UPFBlueprintFunctionLibrary::SetStaticMeshColor(StaticMeshComponent, GetOwnerColor(), 0);
	}

	const FVector ActorLocation = GetActorLocation();

	// Set location to ground
	if (bAdjustLocationToGround)
	{
		FHitResult Hit;
		GetWorld()->LineTraceSingleByChannel(Hit, ActorLocation, ActorLocation + FVector::DownVector * 100,
		                                     ECC_Visibility);
		if (Hit.bBlockingHit)
		{
			SetActorLocation(Hit.Location + FVector::UpVector + LocationToGroundOffset);
		}
	}

	FHitResult Hit;
	GetWorld()->LineTraceSingleByChannel(Hit, ActorLocation + FVector::UpVector * 100, ActorLocation + FVector::DownVector * 100, ECC_Camera);
	if (Hit.bBlockingHit)
	{
		StateWidgetComponent->SetRelativeLocation(Hit.Location + FVector::UpVector - ActorLocation);
	}
}

void APFPawn::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, OwnerPlayer);
	DOREPLIFETIME(ThisClass, CurrentHealth);
	DOREPLIFETIME(ThisClass, MaxHealth);
	DOREPLIFETIME(ThisClass, Attack);
	DOREPLIFETIME(ThisClass, AttackSpeed);
	DOREPLIFETIME(ThisClass, Defense);
}

void APFPawn::SetOwner(AActor* NewOwner)
{
	if (OwnerPlayer)
	{
		OwnerPlayer->OnPlayerOwnedPawnRemoved(this);
	}

	Super::SetOwner(NewOwner);

	ACommanderPawn* CommanderPawn = Cast<ACommanderPawn>(NewOwner);
	if (CommanderPawn != nullptr)
	{
		// DEBUG_MESSAGE(TEXT("Set [%s]'s Owner To [%s]"), *GetName(), *CommanderPawn->GetName());
		OwnerPlayer = CommanderPawn->GetPlayerState<ABattlePlayerState>();

		if (OwnerPlayer)
		{
			OwnerPlayer->OnPlayerOwnedPawnAdd(this);
		}

		OnRep_OwnerPlayer();
	}
}

EPawnRole APFPawn::GetPawnRole(APFPawn* OtherPawn) const
{
	if (OtherPawn == nullptr)
	{
		return EPawnRole::None;
	}

	const APFPlayerState* OtherOwnerPlayer = OtherPawn->GetOwnerPlayer();
	if (OwnerPlayer == nullptr || OtherOwnerPlayer == nullptr)
	{
		return EPawnRole::Neutrality;
	}

	if (OwnerPlayer == OtherOwnerPlayer)
	{
		return EPawnRole::Self;
	}

	if (OwnerPlayer->GetTeamId() == OtherOwnerPlayer->GetTeamId())
	{
		return EPawnRole::Teammate;
	}

	return EPawnRole::None;
}

void APFPawn::OnRep_OwnerPlayer()
{
	// DEBUG_MESSAGE(TEXT("%s__Pawn [%s] Set OwnerPlayer [%s]"), GetLocalRole() == ROLE_Authority ? TEXT("Server") : TEXT("Client"), *GetName(), OwnerPlayer ? *OwnerPlayer->GetPlayerName() : TEXT("NULL"));
	UPFBlueprintFunctionLibrary::SetStaticMeshColor(StaticMeshComponent, GetOwnerColor(), 0);
}

void APFPawn::OnSelected(ACommanderPawn* SelectCommander)
{
	bSelected = true;
	StaticMeshComponent->SetRenderCustomDepth(true);
	// UPFBlueprintFunctionLibrary::SetStaticMeshColor(StaticMeshComponent, GetDefault<UPFGameSettings>()->PawnSelectedColor);
}

void APFPawn::OnDeselected()
{
	bSelected = false;
	StaticMeshComponent->SetRenderCustomDepth(false);
	// UPFBlueprintFunctionLibrary::SetStaticMeshColor(StaticMeshComponent, GetOwnerColor());
}

void APFPawn::OnTarget(class ACommanderPawn* TargetCommander)
{
	AsyncTask(ENamedThreads::Type::AnyThread, [this, TargetCommander]()
	{
		for (int32 i = 0; i < 2; i++)
		{
			UPFBlueprintFunctionLibrary::SetStaticMeshColor(StaticMeshComponent, FLinearColor::Yellow);
			FPlatformProcess::Sleep(0.1f);
			UPFBlueprintFunctionLibrary::SetStaticMeshColor(StaticMeshComponent, GetOwnerColor());
			FPlatformProcess::Sleep(0.1f);
		}
	});
}

float APFPawn::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
                          class AController* EventInstigator, AActor* DamageCauser)
{
	// do not modify damage parameters after this
	const int ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (ActualDamage != 0.0f)
	{
		CurrentHealth = FMath::Max(0, CurrentHealth - ActualDamage);
	}

	return ActualDamage;
}

bool APFPawn::ShouldTakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator,
                               AActor* DamageCauser) const
{
	if (!Super::ShouldTakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser))
	{
		return false;
	}

	return MaxHealth > 0;
}

float APFPawn::InternalTakePointDamage(float Damage, struct FPointDamageEvent const& PointDamageEvent,
                                       class AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamage = Super::InternalTakePointDamage(Damage, PointDamageEvent, EventInstigator, DamageCauser);

	ActualDamage = FMath::Max(1, FMath::CeilToInt(ActualDamage / Defense));

	return ActualDamage;
}

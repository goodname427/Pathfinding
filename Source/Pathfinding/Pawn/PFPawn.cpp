// Fill out your copyright notice in the Description page of Project Settings.

#include "PFPawn.h"

#include "CommanderPawn.h"
#include "PFBlueprintFunctionLibrary.h"
#include "PFUtils.h"
#include "WidgetSettings.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

FName APFPawn::StateWidgetClassName = FName("PFPawnStateBar");
FName APFPawn::PawnBounds_ProfileName = FName("PawnBounds");

// Sets default values
APFPawn::APFPawn()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	INIT_DEFAULT_SUBOBJECT(RootComponent);
	RootComponent->SetMobility(EComponentMobility::Movable);

	// Collision
	INIT_DEFAULT_SUBOBJECT(StaticMeshComponent);
	StaticMeshComponent->SetupAttachment(RootComponent);
	StaticMeshComponent->CanCharacterStepUpOn = ECB_No;
	StaticMeshComponent->SetCollisionProfileName(UCollisionProfile::Pawn_ProfileName);
	StaticMeshComponent->AlwaysLoadOnClient = true;
	StaticMeshComponent->AlwaysLoadOnServer = true;
	StaticMeshComponent->bCastDynamicShadow = true;
	StaticMeshComponent->bAffectDynamicIndirectLighting = true;

	INIT_DEFAULT_SUBOBJECT(BoxComponent);
	BoxComponent->SetupAttachment(RootComponent);
	BoxComponent->SetCollisionProfileName(PawnBounds_ProfileName);
	BoxComponent->SetBoxExtent(FVector(0, 0, 0));
	BoxComponent->SetCanEverAffectNavigation(false);
	BoxComponent->bDynamicObstacle = false;

	// Adjust Location
	bAdjustLocationToGround = true;
	LocationToGroundOffset = 0;

	// State
	// INIT_DEFAULT_SUBOBJECT(StateWidgetComponent);
	// StateWidgetComponent->SetRelativeLocation(FVector::UpVector * 2);
	// StateWidgetComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	// if (const TSubclassOf<UUserWidget>* WidgetClassPtr = GetDefault<UWidgetSettings>()->WidgetClasses.Find(
	// 	StateWidgetClassName))
	// {
	// 	StateWidgetComponent->SetWidgetClass(*WidgetClassPtr);
	// }

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

	SetColor(GetOwnerColor());
	SetBoxComponentToBounds();

	const FVector ActorLocation = GetActorLocation();

	// Set location to ground
	if (bAdjustLocationToGround)
	{
		FHitResult Hit;
		GetWorld()->LineTraceSingleByChannel(
			Hit,
			ActorLocation + FVector::UpVector * 100.0f,
			ActorLocation + FVector::DownVector * 100.0f,
			ECC_Visibility
		);

		if (Hit.bBlockingHit)
		{
			SetActorLocation(Hit.Location + FVector::UpVector + LocationToGroundOffset);
		}
	}

	// FHitResult Hit;
	// GetWorld()->LineTraceSingleByChannel(Hit, ActorLocation + FVector::UpVector * 100, ActorLocation + FVector::DownVector * 100, ECC_Camera);
	// if (Hit.bBlockingHit)
	// {
	// 	DEBUG_MESSAGE(TEXT("%s"), *(Hit.Location + FVector::UpVector - ActorLocation).ToString());
	// 	StateWidgetComponent->SetRelativeLocation(Hit.Location + FVector::UpVector - ActorLocation);
	// }
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

float APFPawn::GetApproximateRadius() const
{
	const FVector& ComponentScale = StaticMeshComponent->GetComponentScale();

	FBox Bounds;
	StaticMeshComponent->GetLocalBounds(Bounds.Min, Bounds.Max);

	FVector Extents = Bounds.GetExtent();
	Extents.X *= ComponentScale.X;
	Extents.Y *= ComponentScale.Y;
	Extents.Z = 0;

	return Extents.Size();
}

void APFPawn::SetBoxComponentToBounds()
{
	const FBox Bounds = GetComponentsBoundingBox();
	BoxComponent->SetBoxExtent(Bounds.GetExtent());
	BoxComponent->SetRelativeLocation(Bounds.GetCenter() - GetActorLocation());
}

void APFPawn::SetOwner(AActor* NewOwner)
{
	const ACommanderPawn* CommanderPawn = Cast<ACommanderPawn>(NewOwner);
	// if (HasAuthority())
	// {
	// 	if (CommanderPawn && OwnerPlayer)
	// 	{
	// 		OwnerPlayer->RemoveOwnedPawn(this);
	// 		OwnerPlayer == nullptr;
	// 	}
	// }

	Super::SetOwner(NewOwner);

	if (HasAuthority())
	{
		if (CommanderPawn)
		{
			// DEBUG_MESSAGE(TEXT("Set [%s]'s Owner To [%s]"), *GetName(), *CommanderPawn->GetName());
			OwnerPlayer = CommanderPawn->GetPlayerState<ABattlePlayerState>();

			if (OwnerPlayer)
			{
				OwnerPlayer->AddOwnedPawn(this);
			}

			OnRep_OwnerPlayer();
		}
	}
}

void APFPawn::SetColor(const FLinearColor& InColor)
{
	UPFBlueprintFunctionLibrary::TryCreateDynamicMaterialInstanceForStaticMesh(
		StaticMeshComponent, GetDefault<UPFGameSettings>()->LoadPawnFlagMaterial());
	UPFBlueprintFunctionLibrary::SetStaticMeshColor(StaticMeshComponent, InColor);
}

EPawnRole APFPawn::GetPawnRole(const APFPawn* OtherPawn) const
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

	return EPawnRole::Enemy;
}

void APFPawn::OnRep_OwnerPlayer()
{
	// DEBUG_MESSAGE(TEXT("%s__Pawn [%s] Set OwnerPlayer [%s]"), GetLocalRole() == ROLE_Authority ? TEXT("Server") : TEXT("Client"), *GetName(), OwnerPlayer ? *OwnerPlayer->GetPlayerName() : TEXT("NULL"));
	SetColor(GetOwnerColor());
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
			SetColor(FLinearColor::Yellow);
			FPlatformProcess::Sleep(0.1f);
			SetColor(GetOwnerColor());
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

		// died
		if (CurrentHealth == 0)
		{
			Die();
		}
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

	return !IsInvincible();
}

void APFPawn::Die_Implementation()
{
	bShouldSkipDied = false;
	if (OnPawnDied.IsBound())
	{
		OnPawnDied.Broadcast(this);
	}

	// Modify the 'bShouldSkipDied' to ture so that skipping died
	if (bShouldSkipDied)
	{
		return;
	}

	if (OwnerPlayer)
	{
		OwnerPlayer->RemoveOwnedPawn(this);
	}
	
	Destroy();
}

float APFPawn::InternalTakePointDamage(float Damage, struct FPointDamageEvent const& PointDamageEvent,
                                       class AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamage = Super::InternalTakePointDamage(Damage, PointDamageEvent, EventInstigator, DamageCauser);

	ActualDamage = FMath::Max(1, FMath::CeilToInt(ActualDamage / Defense));

	return ActualDamage;
}

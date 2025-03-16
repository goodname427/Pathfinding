// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EffectUtils.h"
#include "PFGameSettings.h"
#include "Battle/BattlePlayerState.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/Pawn.h"
#include "PFPawn.generated.h"

class UBoxComponent;

USTRUCT(BlueprintType)
struct FPFPawnEffectData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FEffectWrapper TakeDamage;
};

USTRUCT(BlueprintType)
struct FPFPawnData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName Name;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(MultiLine=true))
	FString Description;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowedClasses="Texture"))
	UObject* Icon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FPFPawnEffectData EffectData;

	FPFPawnData()
	{
		Name = NAME_None;
		Icon = nullptr;

		EffectData = FPFPawnEffectData();
	}
};

UENUM()
enum class EPawnRole : uint8
{
	None,
	Self,
	Neutrality,
	Teammate,
	Enemy,
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPawnDiedSignature, APFPawn*, DiedPawn);

UCLASS()
class PATHFINDING_API APFPawn : public APawn
{
	GENERATED_BODY()

public:
	APFPawn();

	virtual void PostInitProperties() override;

	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

public:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

public:
	static FName PawnBounds_ProfileName;

	UFUNCTION(BlueprintCallable)
	virtual float GetApproximateRadius() const;

	UFUNCTION(BlueprintCallable)
	UStaticMeshComponent* GetStaticMeshComponent() const { return StaticMeshComponent; }

	UFUNCTION(BlueprintCallable)
	UBoxComponent* GetBoxComponent() const { return BoxComponent; }

	UFUNCTION(BlueprintCallable)
	void SetBoxComponentToBounds();

protected:
	UPROPERTY(Category = "Pawn", VisibleAnywhere, BlueprintReadOnly)
	UBoxComponent* BoxComponent;

	// Static Mesh
	UPROPERTY(Category = "StaticMesh", VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* StaticMeshComponent;

	UPROPERTY(Category = "StaticMesh | AdjustLocation", EditAnywhere, BlueprintReadOnly)
	uint32 bAdjustLocationToGround : 1;

	UPROPERTY(Category = "StaticMesh | AdjustLocation", EditAnywhere, BlueprintReadOnly)
	float LocationToGroundOffset;

public:
	// Owner
	virtual void SetOwner(AActor* NewOwner) override;

	UFUNCTION(BlueprintCallable)
	ABattlePlayerState* GetOwnerPlayer() const { return OwnerPlayer; }

	UFUNCTION(BlueprintCallable)
	FLinearColor GetOwnerColor() const
	{
		return OwnerPlayer ? OwnerPlayer->GetPlayerColor() : GetDefault<UPFGameSettings>()->PawnNormalColor;
	}

	virtual void SetColor(const FLinearColor& InColor);

	UFUNCTION(BlueprintCallable)
	EPawnRole GetPawnRole(const APFPawn* OtherPawn) const;

protected:
	UPROPERTY(Transient, ReplicatedUsing = OnRep_OwnerPlayer)
	ABattlePlayerState* OwnerPlayer;

	UFUNCTION()
	void OnRep_OwnerPlayer();

public:
	// Select
	virtual void OnSelected(class ACommanderPawn* SelectCommander);
	virtual void OnDeselected();
	virtual void OnTarget(class ACommanderPawn* TargetCommander);

	bool HasSelected() const { return bSelected; }

private:
	UPROPERTY(Transient, Category = "Select", VisibleAnywhere)
	uint32 bSelected : 1;

public:
	UPROPERTY(BlueprintAssignable)
	FPawnDiedSignature OnPawnPreDied;

	UPROPERTY(Transient, BlueprintReadWrite)
	bool bShouldSkipDied;

public:
	// Damage
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
	                         class AController* EventInstigator, AActor* DamageCauser) override;;

	virtual bool ShouldTakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator,
	                              AActor* DamageCauser) const override;

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void Die();

	virtual void OnDied() {}
	
	// To avoid the unexpected order of function 
	UFUNCTION(BlueprintCallable)
	void DieDelay(float DelayDuration = 0.1f);

protected:
	virtual float InternalTakePointDamage(float Damage, struct FPointDamageEvent const& PointDamageEvent,
	                                      class AController* EventInstigator, AActor* DamageCauser) override;

public:
	// State
	const FPFPawnData& GetData() const { return Data; }

	UFUNCTION(BlueprintCallable)
	bool CanAttack() const { return Attack > 0 && AttackSpeed > 0; }

	UFUNCTION(BlueprintCallable)
	bool HasDied() const { return CurrentHealth == 0; }

	UFUNCTION(BlueprintCallable)
	bool IsInvincible() const { return MaxHealth <= 0 || Defense <= 0; }

	float GetCurrentHealth() const { return CurrentHealth; }

	float GetAttack() const { return Attack; }

	float GetAttackSpeed() const { return AttackSpeed; }

	float GetAttackDuration() const { return AttackSpeed <= 0 ? 0 : 1.f / AttackSpeed; }

	float GetAttackRadius() const { return AttackRadius; }

	float GetMoveSpeed() const { return MoveSpeed; }

protected:
	UPROPERTY(Category = "State", EditDefaultsOnly, BlueprintReadOnly)
	FPFPawnData Data;

	UPROPERTY(Transient, Category = "State", VisibleAnywhere, BlueprintReadWrite, Replicated)
	int32 CurrentHealth;

	UPROPERTY(Category = "State", EditAnywhere, BlueprintReadWrite, Replicated, meta = (ClampMin = 0))
	int32 MaxHealth;

	UPROPERTY(Category = "State", EditAnywhere, BlueprintReadWrite, Replicated, meta = (ClampMin = 0))
	int32 Defense;
	
	UPROPERTY(Category = "State", EditAnywhere, BlueprintReadWrite, Replicated, meta = (ClampMin = 0))
	int32 Attack;

	UPROPERTY(Category = "State", EditAnywhere, BlueprintReadWrite, Replicated, meta = (ClampMin = 0))
	float AttackSpeed;

	UPROPERTY(Category = "State" , EditAnywhere, BlueprintReadWrite, Replicated, meta = (ClampMin = 0))
	float AttackRadius;

	UPROPERTY(Category = "State", EditAnywhere, BlueprintReadWrite, Replicated, meta = (ClampMin = 0))
	float MoveSpeed;

public:
	// Widget
	UFUNCTION(Unreliable, NetMulticast)
	void StartShowStateWidget();

protected:
	static FName StateWidgetClassName;

	UPROPERTY(Category = "Widget", VisibleAnywhere, BlueprintReadOnly)
	UWidgetComponent* StateWidgetComponent;

	UPROPERTY(Category = "Widget", EditDefaultsOnly, BlueprintReadOnly)
	float StateWidgetHeightRatio;

	UPROPERTY(Category = "Widget", EditDefaultsOnly, BlueprintReadOnly)
	float StateWidgetScaleRatio;

private:
	float StateWidgetHideTimer;

public:
	UFUNCTION(BlueprintCallable)
	void PlaySound(USoundBase* Sound);

protected:
	UPROPERTY(Category = "Sound", VisibleAnywhere, BlueprintReadOnly)
	UAudioComponent* AudioComponent;
};

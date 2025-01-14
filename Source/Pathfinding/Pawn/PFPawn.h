// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PFGameSettings.h"
#include "Battle/BattlePlayerState.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/Pawn.h"
#include "PFPawn.generated.h"

USTRUCT(BlueprintType)
struct FPFPawnData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName Name;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString Description;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowedClasses="Texture"))
	UObject* Icon;

	FPFPawnData()
	{
		Name = NAME_None;
		Icon = nullptr;
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

UCLASS()
class PATHFINDING_API APFPawn : public APawn
{
	GENERATED_BODY()

public:
	APFPawn();

	virtual void PostInitProperties() override;

protected:
	virtual void BeginPlay() override;

public:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

public:
	UFUNCTION(BlueprintCallable)
	virtual float GetApproximateRadius() const;
	
	UFUNCTION(BlueprintCallable)
	UStaticMeshComponent* GetStaticMeshComponent() const { return StaticMeshComponent; }
	
protected:
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
	
private:
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
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
	                         class AController* EventInstigator, AActor* DamageCauser) override;;

	virtual bool ShouldTakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator,
	                              AActor* DamageCauser) const override;

	UFUNCTION(BlueprintCallable, Server, Reliable)
	virtual void Die();

protected:
	virtual float InternalTakePointDamage(float Damage, struct FPointDamageEvent const& PointDamageEvent,
	                                      class AController* EventInstigator, AActor* DamageCauser) override;

public:
	const FPFPawnData& GetData() const { return Data; }

	UFUNCTION(BlueprintCallable)
	bool CanAttack() const { return Attack > 0 && AttackSpeed > 0;  }
	
	float GetAttack() const { return Attack; }
	
	float GetAttackSpeed() const { return AttackSpeed; }

	float GetAttackDuration() const { return 1.f / AttackSpeed; }
	
protected:
	UPROPERTY(Category = "State", EditDefaultsOnly, BlueprintReadOnly)
	FPFPawnData Data;

	UPROPERTY(Transient, Category = "State", VisibleAnywhere, BlueprintReadWrite, Replicated)
	int32 CurrentHealth;
	
	UPROPERTY(Category = "State", EditAnywhere, BlueprintReadWrite, Replicated, meta = (ClampMin = 0))
	int32 MaxHealth;

	UPROPERTY(Category = "State", EditAnywhere, BlueprintReadWrite, Replicated, meta = (ClampMin = 0))
	int32 Attack;

	UPROPERTY(Category = "State", EditAnywhere, BlueprintReadWrite, Replicated, meta = (ClampMin = 0))
	float AttackSpeed;

	UPROPERTY(Category = "State", EditAnywhere, BlueprintReadWrite, Replicated, meta = (ClampMin = 0))
	int32 Defense;

protected:
	static FName StateWidgetClassName;
	
	UPROPERTY(Category = "State", VisibleAnywhere, BlueprintReadOnly)
	UWidgetComponent* StateWidgetComponent;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PFGameSettings.h"
#include "PFPlayerState.h"
#include "GameFramework/Pawn.h"
#include "PFPawn.generated.h"

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

protected:
	virtual void BeginPlay() override;

public:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	// Static Mesh
	UPROPERTY(Category = "StaticMesh", EditDefaultsOnly)
	UStaticMeshComponent* StaticMeshComponent;
	
public:
	// Owner
	virtual void SetOwner(AActor* NewOwner) override;
	
	UFUNCTION(BlueprintCallable)
	APFPlayerState* GetOwnerPlayer() const { return OwnerPlayer; }

	UFUNCTION(BlueprintCallable)
	FLinearColor GetOwnerColor() const { return OwnerPlayer ? OwnerPlayer->GetPlayerColor() : GetDefault<UPFGameSettings>()->PawnNormalColor; }

	UFUNCTION(BlueprintCallable)
	EPawnRole GetPawnRole(APFPawn* OtherPawn) const;
	
private:
	UPROPERTY(Transient, ReplicatedUsing = OnRep_OwnerPlayer)
	APFPlayerState* OwnerPlayer;

	UFUNCTION()
	void OnRep_OwnerPlayer();

public:
	// Select
	virtual void OnSelected(class ACommanderPawn* SelectCommander);
	virtual void OnDeselected();
	
	bool HasSelected() const { return bSelected; }

private:
	UPROPERTY(Transient, Category = "Select", VisibleAnywhere)
	uint32 bSelected : 1;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Conscious/ConsciousPawn.h"
#include "PFPawn.h"
#include "GameFramework/Pawn.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Movement/CommanderPawnMovementComponent.h"
#include "CommanderPawn.generated.h"

UCLASS()
class PATHFINDING_API ACommanderPawn : public APawn
{
	GENERATED_BODY()

public:
	ACommanderPawn();

	virtual void PostInitProperties() override;

protected:
	virtual void BeginPlay() override;
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
protected:
	UPROPERTY(Category = "Camera", VisibleAnywhere)
	class UCameraComponent* CameraComponent;

	UPROPERTY(Category = "Camera", VisibleAnywhere)
	class USpringArmComponent* SpringArmComponent;

	// Static mesh
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UStaticMeshComponent* StaticMeshComponent;

protected:
	// Camera Scale
	void CameraScale(float Value);

	UPROPERTY(Category = "Control|CameraScale", EditDefaultsOnly)
	float MinTargetArmLength;

	UPROPERTY(Category = "Control|CameraScale", EditDefaultsOnly)
	float MaxTargetArmLength;

	UPROPERTY(Category = "Control|CameraScale", EditDefaultsOnly)
	float CameraScaleSpeed;

private:
	float CameraScaleValue;

protected:
	// Moving
	bool IsMouseOnScreenEdge(FVector2D& OutMousePositionOnEdge) const;
	void Move(float Value, EAxis::Type InAxis);
	void MoveVertical(float Value);
	void MoveHorizontal(float Value);

	virtual UPawnMovementComponent* GetMovementComponent() const override { return MovementComponent; }

	UPROPERTY(Category = "Control|Move", VisibleAnywhere)
	UPawnMovementComponent* MovementComponent;

	UPROPERTY(Category = "Control|Move", EditDefaultsOnly)
	uint32 bEnableMouseMove : 1;

protected:
	// Camera Rotate
	void ControlPressed();
	void ControlReleased();
	void MouseHorizontal(float Value);

	UPROPERTY(Category = "Control|CameraRotate", EditDefaultsOnly)
	float CameraRotateSpeed;

private:
	UPROPERTY(Transient, Category = "Control|CameraRotate", VisibleAnywhere)
	uint32 bControlPressed : 1;

protected:
	// Select
	void SelectPressed();
	void SelectReleased();
	void Select_CtrlPressed();
	void Select_CtrlReleased();
	
	void SelectDoubleClick();
	
	void BeginSelect();
	void EndSelect(bool bAdditional, bool bSkipSelect = false);
	
	void Select(APFPawn* Pawn);
	void Deselect(APFPawn* Pawn);
	void DeselectAll();

	UFUNCTION(Server, Reliable)
	void ServerSelect(APFPawn* Pawn);
	UFUNCTION(Server, Reliable)
	void ServerDeselect(APFPawn* Pawn);
	UFUNCTION(Server, Reliable)
	void ServerDeselectAll();
	
	
	void LineTrace(const APlayerController* Player, const FVector2D& ScreenPoint, FHitResult& OutResult) const;

	struct FMultiLineTraceResult
	{
		TSet<APFPawn*> HitPawns;
		bool bHasOwned;
		APFPawn* FirstOthersPawn;
	};
	void SelectBoxLineTracePawn(const APlayerController* PlayerController, const FBox2D& SelectBox, FMultiLineTraceResult& OutResult) const;

private:
	uint32 bDoubleClick : 1;
	
	UPROPERTY(Transient, Category = "Select", VisibleAnywhere)
	uint32 bSelectPressed : 1;

	float LineTraceStepFactor;

	// by cm
	UPROPERTY(Category = "Select|LineTrace", EditDefaultsOnly)
	float LineTraceDistance;

public:
	UFUNCTION(BlueprintCallable)
	bool IsOwned(APFPawn* Pawn) const;
	
	UFUNCTION(BlueprintCallable)
	APFPawn* GetFirstSelectedPawn() const { return SelectedPawns.Num() > 0 ? SelectedPawns[0] : nullptr; };

private:
	UPROPERTY(Transient, Category = "Select", VisibleAnywhere, Replicated)
	TArray<APFPawn*> SelectedPawns;

public:
	UFUNCTION(BlueprintCallable, Server, Reliable)
	void Send(const FTargetRequest& Request);

protected:
	// target
	void TargetPressed();
	void TargetReleased();


public:
	UFUNCTION(BlueprintNativeEvent)
	void Test();

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void SpawnPawn(TSubclassOf<APFPawn> PawnClass, FVector Location);
};

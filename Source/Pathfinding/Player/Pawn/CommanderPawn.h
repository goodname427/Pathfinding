// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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
	// Sets default values for this pawn's properties
	ACommanderPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
protected:
	UPROPERTY(Category = "Camera", VisibleAnywhere)
	class UCameraComponent* Camera;

	UPROPERTY(Category = "Camera", VisibleAnywhere)
	class USpringArmComponent* SpringArm;

	// Static mesh
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UStaticMeshComponent* StaticMesh;

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
	bool IsMouseOnScreenEdge(FVector2D& OutMousePositionOnEdge);
	void Move(float Value, EAxis::Type InAxis);
	void MoveVertical(float Value);
	void MoveHorizontal(float Value);

	virtual UPawnMovementComponent* GetMovementComponent() const override { return Movement; }

	UPROPERTY(Category = "Control|Move", VisibleAnywhere)
	UPawnMovementComponent* Movement;

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
	void EndSelect(bool bAdditional);

	void Select(APFPawn* PFPawn);
	void Deselect(APFPawn* PFPawn);
	void DeselectAll();
	
	APFPawn* LineTrace(APlayerController* Player, FVector2D ScreenPoint);
	void MultiLineTrace(TSet<APFPawn*>& HitPawns, APlayerController* PlayerController, const FBox2D& SelectBox, bool& bHasOwned, APFPawn*& FirstOthersPawn);


private:
	UPROPERTY(Transient, Category = "Select", VisibleAnywhere)
	uint32 bSelectPressed : 1;
	
	UPROPERTY(Category = "Select|LineTrace", EditDefaultsOnly)
	float LineTraceStep;

	UPROPERTY(Category = "Select|LineTrace", EditDefaultsOnly)
	float LineTraceDistance;

	UPROPERTY(Transient, Category = "Select", VisibleAnywhere, Replicated)
	TArray<APFPawn*> SelectedPawns;

protected:
	// target
	void TargetPressed();
	void TargetReleased();

public:
	UFUNCTION(BlueprintNativeEvent)
	void Test();

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void SpawnPFPawn(TSubclassOf<APFPawn> PawnClass, FVector Location);
};

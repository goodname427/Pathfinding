// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ConsciousPawn.h"
#include "PFPawn.h"
#include "GameFramework/Pawn.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Movement/CommanderPawnMovementComponent.h"
#include "CommanderPawn.generated.h"

UENUM(BlueprintType)
enum class ECommanderState: uint8
{
	None,
	Selecting,
	Targeting,
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSelectedPawnChangedSignature, ACommanderPawn*, CommanderPawn);

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
	
public:
	// Select
	UFUNCTION(BlueprintCallable)
	void Select(APFPawn* Pawn);

	UFUNCTION(BlueprintCallable)
	void Deselect(APFPawn* Pawn);

	UFUNCTION(BlueprintCallable)
	void DeselectAll();
	
	FRay GetRayFromMousePosition() const;

protected:
	void SelectPressed();
	void SelectReleased();
	void SelectDoubleClick();

	void Select_CtrlPressed();
	void Select_CtrlReleased();

	void BeginSelect();
	void EndSelect(bool bAdditional, bool bSkipSelect = false);
	
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

	void SelectBoxLineTracePawn(const APlayerController* PlayerController, const FBox2D& SelectBox,
	                            FMultiLineTraceResult& OutResult) const;

public:
	bool IsSelecting() const { return bSelecting; }

private:
	uint32 bDoubleClick : 1;
	uint32 bSelecting : 1;
	FVector2D SelectBoxBeginMousePos;;

protected:
	float LineTraceStepFactor;
	// by cm
	UPROPERTY(Category = "Select|LineTrace", EditDefaultsOnly)
	float LineTraceDistance;

public:
	UFUNCTION(BlueprintCallable)
	bool IsOwned(APFPawn* Pawn) const;

	UFUNCTION(BlueprintCallable)
	APFPawn* GetFirstSelectedPawn() const { return SelectedPawns.Num() > 0 ? SelectedPawns[0] : nullptr; };

	UFUNCTION(BlueprintCallable)
	const TArray<APFPawn*>& GetSelectedPawns() const { return SelectedPawns; };

	UFUNCTION(BlueprintCallable)
	const TArray<APFPawn*>& GetSortedSelectedPawns() const;

public:
	UPROPERTY(BlueprintAssignable, Category = "Select")
	FSelectedPawnChangedSignature OnSelectedPawnChanged;
	
private:
	UPROPERTY(Transient, Category = "Select", VisibleAnywhere, ReplicatedUsing=OnRep_SelectedPawn)
	TArray<APFPawn*> SelectedPawns;
	UFUNCTION()
	void OnRep_SelectedPawn() {if (OnSelectedPawnChanged.IsBound()) OnSelectedPawnChanged.Broadcast(this); }

public:
	// target
	UFUNCTION(BlueprintCallable, Server, Reliable)
	void Send(const FTargetRequest& Request);

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void SendTo(const FTargetRequest& Request, AConsciousPawn* ReceivedPawn);

	UFUNCTION(BlueprintCallable)
	void BeginTarget(UCommandComponent* InTargetingCommand);

	UFUNCTION(BlueprintCallable)
	void EndTarget(bool bCanceled);

protected:
	void TargetPressed();
	void TargetReleased();

	void Target(UCommandComponent* Command = nullptr);

public:
	bool IsTargeting() const { return bTargeting; };
	FName GetTargetingCommandName() const { return TargetingCommand->GetCommandName(); }

private:
	uint32 bTargeting : 1;

	UPROPERTY(Transient)
	UCommandComponent* TargetingCommand;

public:
	UFUNCTION(BlueprintNativeEvent)
	void Test();

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void ServerSpawnPawn(TSubclassOf<APFPawn> PawnClass, FVector Location);

	APFPawn* SpawnPawn(TSubclassOf<APFPawn> PawnClass, FVector Location);

	template<class T>
	T* SpawnPawn(TSubclassOf<APFPawn> PawnClass, FVector Location) { return Cast<T>(SpawnPawn(PawnClass, Location)); }
	
	APFPawn* SpawnPawnFrom(APFPawn* Source, TSubclassOf<APFPawn> PawnClassToSpawn);
	
	template<class T>
	T* SpawnPawnFrom(APFPawn* Source, TSubclassOf<APFPawn> PawnClassToSpawn) { return Cast<T>(SpawnPawnFrom(Source, PawnClassToSpawn)); }
};

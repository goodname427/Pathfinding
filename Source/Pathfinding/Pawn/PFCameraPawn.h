// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include <PFCameraPawnMovementComponent.h>
#include "PFCameraPawn.generated.h"


UCLASS()
class PATHFINDING_API APFCameraPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	APFCameraPawn(const FObjectInitializer& ObjectInitializer);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	// Camera
	void CameraScale(float Value);

	UPROPERTY(Category = "Camera", VisibleAnywhere)
	class UCameraComponent* Camera;
	
	UPROPERTY(Category = "Camera", VisibleAnywhere)
	class USpringArmComponent* SpringArm;

	UPROPERTY(Category = "Camera", EditDefaultsOnly)
	float MinTargetArmLength;

	UPROPERTY(Category = "Camera", EditDefaultsOnly)
	float MaxTargetArmLength;

	UPROPERTY(Category = "Camera", EditDefaultsOnly)
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

	UPROPERTY(Category = "Move", VisibleAnywhere)
	UPawnMovementComponent* Movement;
	
	UPROPERTY(Category = "Move", EditDefaultsOnly)
	uint32 bEnableMouseMove : 1;

protected:
	UPROPERTY(EditDefaultsOnly)
	UStaticMeshComponent* StaticMesh;
};

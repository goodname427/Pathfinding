// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Command/CommandComponent.h"
#include "Command/ProgressCommandComponent.h"
#include "ConsciousAIController.generated.h"



UCLASS()
class PATHFINDING_API AConsciousAIController : public AAIController
{
	GENERATED_BODY()

public:
	AConsciousAIController();
};

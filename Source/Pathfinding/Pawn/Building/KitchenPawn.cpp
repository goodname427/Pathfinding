// Fill out your copyright notice in the Description page of Project Settings.


#include "KitchenPawn.h"


// Sets default values
AKitchenPawn::AKitchenPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	INIT_DEFAULT_SUBOBJECT(ProduceCommandComponent);
}

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camp.h"
#include "Engine/DeveloperSettings.h"
#include "PFGameSettings.generated.h"


USTRUCT(BlueprintType)
struct FMapInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FString MapName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FString MapDescription;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(AllowedClasses="World"))
	FSoftObjectPath MapPath;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	int32 MaxPlayers;
};

/**
 * 
 */
UCLASS(Config = Game, defaultconfig)
class PATHFINDING_API UPFGameSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(Config, EditDefaultsOnly, BlueprintReadOnly, Category = "Map")
	TArray<FMapInfo> Maps;

public:
	UPROPERTY(Config, EditDefaultsOnly, BlueprintReadOnly, Category = "Camp")
	TArray<TSubclassOf<UCamp>> CampClasses;

	UFUNCTION(BlueprintCallable, Category = "Camp")
	const UCamp* GetRandomlyCamp() const;
	
public:
	UPROPERTY(Config, EditDefaultsOnly, BlueprintReadOnly, Category = "Color")
	TArray<FLinearColor> PlayerColors;

	UPROPERTY(Config, EditDefaultsOnly, BlueprintReadOnly, Category = "Color|Select")
	FLinearColor PawnSelectedColor;

	UPROPERTY(Config, EditDefaultsOnly, BlueprintReadOnly, Category = "Color|Select")
	FLinearColor PawnNormalColor;

public:
	UPROPERTY(Config, EditDefaultsOnly, BlueprintReadOnly, Category = "Pawn")
	float PawnMinSize;

	UPROPERTY(Config, EditDefaultsOnly, BlueprintReadOnly, Category = "Pawn", meta = (AllowedClasses = "Material"))
	FSoftObjectPath PawnFlagMaterial;
};

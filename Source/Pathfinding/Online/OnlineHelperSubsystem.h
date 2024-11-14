// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OnlineSessionSettings.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "OnlineHelperSubsystem.generated.h"


USTRUCT(BlueprintType)
struct FSessionSearchResult
{
	GENERATED_USTRUCT_BODY()

	FSessionSearchResult();
	FSessionSearchResult(const FOnlineSessionSearchResult& InResult);

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString SessionName;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 PingInMS;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 NumCurrentPlayer;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 NumMaxPlayer;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FFindRoomsSignature, const TArray<FSessionSearchResult>&, SessionSearchResults);

/**
 * 
 */
UCLASS()
class PATHFINDING_API UOnlineHelperSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

public:
	UPROPERTY(BlueprintAssignable)
	FFindRoomsSignature OnFindRooms;

protected:
	FUniqueNetIdPtr GetPlayerUniqueNetId() const;
	
public:
	// Network
	UFUNCTION(BlueprintCallable)
	void DestroySession();

	UFUNCTION(BlueprintCallable)
	void HostRoom();

	UFUNCTION(BlueprintCallable)
	void JoinRoom(int32 RoomIndex);

	UFUNCTION(BlueprintCallable)
	void FindRooms();

	void TravelToRoom();

private:
	TSharedRef<FOnlineSessionSearch> SearchSettings = MakeShareable(new FOnlineSessionSearch());;
};

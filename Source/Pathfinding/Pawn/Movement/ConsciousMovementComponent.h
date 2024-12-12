#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PawnMovementComponent.h"
#include "ConsciousMovementComponent.generated.h"

USTRUCT(BlueprintType)
struct FConsciousMoveData
{
    GENERATED_BODY()

    UPROPERTY()
    float TimeStamp;
    
    UPROPERTY()
    FVector Location;
    
    UPROPERTY()
    FVector Velocity;
    
    UPROPERTY()
    FVector MoveDirection;
};

UCLASS(ClassGroup=(Movement), meta=(BlueprintSpawnableComponent))
class PATHFINDING_API UConsciousMovementComponent : public UPawnMovementComponent
{
    GENERATED_BODY()

public:
    UConsciousMovementComponent();

    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    
    // Movement parameters
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement")
    float MaxSpeed = 600.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement")
    float Acceleration = 2000.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement")
    float Deceleration = 2000.0f;

    // AI Movement Interface
    UFUNCTION(BlueprintCallable, Category = "AI|Movement")
    void RequestDirectMove(const FVector& MoveVelocity, bool bForceMaxSpeed);
    
protected:
    // Movement state
    UPROPERTY(Replicated)
    FVector CurrentVelocity;
    
    UPROPERTY(Replicated)
    FVector RequestedVelocity;
    
    // Network prediction
    TArray<FConsciousMoveData> SavedMoves;
    float LastServerTimeStamp;
    
    // Movement functions
    void PerformMovement(float DeltaTime);
    void SimulateMovement(float DeltaTime);
    
    // Network functions
    UFUNCTION(Server, Reliable)
    void ServerMove(const FConsciousMoveData& MoveData);
    
    UFUNCTION(NetMulticast, Reliable)
    void MulticastMove(const FVector& NewLocation, const FVector& NewVelocity);
    
    void SaveMove(const FConsciousMoveData& NewMove);
    void ClearSavedMoves();
}; 
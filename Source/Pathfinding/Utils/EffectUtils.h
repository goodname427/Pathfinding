#pragma once

#include "CoreMinimal.h"
#include "PFUtils.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "EffectUtils.generated.h"

USTRUCT(BlueprintType)
struct FEffectWrapper
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	USoundBase* Sound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UParticleSystem* Particle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float ParticleScale = 1;
};

inline void PlayEffect(AActor* ActorToPlay, const FEffectWrapper& Effect)
{
	if (!ActorToPlay)
	{
		return;
	}

	
	//DEBUG_MESSAGE(TEXT("Play Sound [%s]"), Effect.Sound ? *Effect.Sound->GetName() : TEXT("NULL"));
	UGameplayStatics::PlaySoundAtLocation(ActorToPlay, Effect.Sound, ActorToPlay->GetActorLocation(), ActorToPlay->GetActorRotation());

	UParticleSystemComponent* PSC = UGameplayStatics::SpawnEmitterAtLocation(ActorToPlay, Effect.Particle, ActorToPlay->GetActorLocation(), ActorToPlay->GetActorRotation());
	if (PSC)
	{
		PSC->SetWorldScale3D(FVector(Effect.ParticleScale));	
	}
}

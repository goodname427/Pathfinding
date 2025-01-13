// Fill out your copyright notice in the Description page of Project Settings.


#include "DelayHelper.h"

#include "PFUtils.h"

void UDelayHelper::Delay(UObject* WorldContextObject, float Duration, TFunction<void()> InFuncToDelayExecute)
{
	UDelayHelper* DelayHelper = NewObject<UDelayHelper>(WorldContextObject);
	DelayHelper->FuncToDelayExecute = InFuncToDelayExecute;

	DelayHelper->AddToRoot();
	
	const FLatentActionInfo LatentInfo(0, FMath::Rand(), TEXT("OnDelayFinished"), DelayHelper);
	UKismetSystemLibrary::Delay(WorldContextObject, Duration, LatentInfo);
}

void UDelayHelper::OnDelayFinished()
{
	// DEBUG_FUNC_FLAG();
	FuncToDelayExecute();
	
	RemoveFromRoot();
}

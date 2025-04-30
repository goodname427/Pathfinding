#pragma once

//
// MACRO
//

#define DOWHILE_WRAP_OPEN \
do \
{
#define DOWHILE_WRAP_CLOSE \
} \
while(0)
//
// LOG
//

#define UE_LOG_TEMP(Format, ...) UE_LOG(LogTemp, Log, Format, __VA_ARGS__)

#define DEBUG_FUNC_FLAG() GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("Call %s"), TEXT(__FUNCTION__)))

#define DEBUG_MESSAGE(Format, ...) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("%s: "), TEXT(__FUNCTION__)) + FString::Printf(Format, __VA_ARGS__))

//
// INIT
//

#define INIT_DEFAULT_SUBOBJECT(Comp) Comp = CreateDefaultSubobject<std::remove_pointer<decltype(Comp)>::type>(TEXT(#Comp))

//
// BIND
//

#define FAST_BIND_AXIS(AxisName) PlayerInputComponent->BindAxis(#AxisName, this, &ThisClass::AxisName)

#define FAST_BIND_ACTION(ActionName) \
DOWHILE_WRAP_OPEN \
	PlayerInputComponent->BindAction(#ActionName, IE_Pressed, this, &ThisClass::ActionName##Pressed); \
	PlayerInputComponent->BindAction(#ActionName, IE_Released, this, &ThisClass::ActionName##Released); \
DOWHILE_WRAP_CLOSE
//
// CHECK
// 

#define SAFE_CHECK(Expr, CategoryName, Format, ...) \
DOWHILE_WRAP_OPEN \
	if (!(Expr))\
	{ \
		UE_LOG(CategoryName, Warning, Format, __VA_ARGS__); \
		return; \
	} \
DOWHILE_WRAP_CLOSE

#define SAFE_CHECK_RET(Expr, Ret, CategoryName, Format, ...) \
DOWHILE_WRAP_OPEN \
	if (!(Expr))\
	{ \
		UE_LOG(CategoryName, Warning, Format, __VA_ARGS__); \
		return Ret; \
	} \
DOWHILE_WRAP_CLOSE

#define SAFE_CHECK_TEMP(Expr, Format, ...) SAFE_CHECK(Expr, LogTemp, Format, __VA_ARGS__)

#define SAFE_CHECK_RET_TEMP(Expr, Ret, Format, ...) SAFE_CHECK_RET(Expr, Ret, LogTemp, Format, __VA_ARGS__)

#define NULL_CHECK(Ptr) SAFE_CHECK_TEMP(Ptr, TEXT("%s Is Null"), #Ptr)

#define NULL_CHECK_RET(Ptr, Ret) SAFE_CHECK_RET_TEMP(Ptr, Ret, TEXT("%s Is Null"), #Ptr)

#define VALID_CHECK(Obj) SAFE_CHECK_TEMP(Obj->IsValidLowLevel(), TEXT("%s Is Not Valid"), #Obj)

#define VALID_CHECK_RET(Obj, Ret) SAFE_CHECK_RET_TEMP(Obj->IsValidLowLevel(), Ret, TEXT("%s Is Not Valid"), #Obj)

template <class TFunc>
void Delay(UObject* WorldContextObject, float Delay, TFunc Func)
{
	// check object
	NULL_CHECK(WorldContextObject);
	NULL_CHECK(GEngine);

	const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::ReturnNull);
	NULL_CHECK(World);

	// use shared ptr to handle timer handle
	TSharedPtr<FTimerHandle> TimerHandle = MakeShared<FTimerHandle>();

	// create timer delegate
	FWeakObjectPtr WeakContext(WorldContextObject);
	FTimerDelegate TimerDelegate;
	TimerDelegate.BindLambda([WeakContext, TimerHandle, Func]()
	{
		// check object
		if (!WeakContext.IsValid())
		{
			return;
		}
		
		// VALID_CHECK(WorldContextObject);
		VALID_CHECK(GEngine);
		
		// get world
		const UWorld* CurrentWorld = GEngine->GetWorldFromContextObject(WeakContext.Get(), EGetWorldErrorMode::ReturnNull);
		VALID_CHECK(CurrentWorld);
		
		// clear timer
		CurrentWorld->GetTimerManager().ClearTimer(*TimerHandle);
		
		// call func
		Func();
	});

	// set timer
	World->GetTimerManager().SetTimer(*TimerHandle, TimerDelegate, Delay, false);
}

#pragma once

//
// MACRO
//

#define DOWHILE_WRAP_OPEN \
do \
{ \

#define DOWHILE_WRAP_CLOSE \
} \
while(0) \

//
// LOG
//

#define UE_LOG_TEMP(Format, ...) UE_LOG(LogTemp, Log, Format, __VA_ARGS__)

//
// INIT
//

#define INIT_DEFAULT_SUBOBJECT(Comp) Comp = CreateDefaultSubobject<std::remove_pointer<decltype(Comp)>::type>(TEXT(#Comp))

//
// BIND
//

#define FAST_BIND_AXIS(AxisName) PlayerInputComponent->BindAxis(#AxisName, this, &ThisClass::##AxisName)

#define FAST_BIND_ACTION(ActionName) \
DOWHILE_WRAP_OPEN \
	PlayerInputComponent->BindAction(#ActionName, IE_Pressed, this, &ThisClass::ActionName##Pressed); \
	PlayerInputComponent->BindAction(#ActionName, IE_Released, this, &ThisClass::ActionName##Released); \
DOWHILE_WRAP_CLOSE \

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
DOWHILE_WRAP_CLOSE \

#define SAFE_CHECK_RET(Expr, Ret, CategoryName, Format, ...) \
DOWHILE_WRAP_OPEN \
	if (!(Expr))\
	{ \
		UE_LOG(CategoryName, Warning, Format, __VA_ARGS__); \
		return Ret; \
	} \
DOWHILE_WRAP_CLOSE \

#define SAFE_CHECK_TEMP(Expr, Format, ...) SAFE_CHECK(Expr, LogTemp, Format, __VA_ARGS__)

#define SAFE_CHECK_RET_TEMP(Expr, Ret, Format, ...) SAFE_CHECK_RET(Expr, Ret, LogTemp, Format, __VA_ARGS__)

#define NULL_CHECK(Ptr) SAFE_CHECK_TEMP(Ptr, TEXT("%s Is Null"), #Ptr)

#define NULL_CHECK_RET(Ptr, Ret) SAFE_CHECK_RET_TEMP(Ptr, Ret, TEXT("%s Is Null"), #Ptr)

//
// ???
//

#define LAZY_LOAD(Class, MemberName, LoadFunc) \
Class* Get##MemberName() \
{ \
	if (MemberName == nullptr) \
	{ \
		MemberName = LoadFunc; \
	} \
	return MemberName; \
} \
Class* MemberName \

#include <functional>

template<typename HandleType>
struct THandleHolder
{
	THandleHolder()
	{}
	THandleHolder(HandleType InHandle)
		: Handle(InHandle)
	{}
	
	void SetHandle(HandleType InHandle) { Handle = InHandle; }
	
private:
	HandleType Handle;
};

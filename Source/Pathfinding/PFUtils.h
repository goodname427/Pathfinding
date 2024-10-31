#pragma once

#define UE_LOG_TEMP(Format, ...) UE_LOG(LogTemp, Log, Format, __VA_ARGS__)

#define InitDefaultSubobject(Comp) Comp = CreateDefaultSubobject<std::remove_pointer<decltype(Comp)>::type>(TEXT(#Comp))

#define FastBindAxis(AxisName) PlayerInputComponent->BindAxis(#AxisName, this, &ThisClass::##AxisName)
#define FastBindAction(ActionName) \
do \
{ \
	PlayerInputComponent->BindAction(#ActionName, IE_Pressed, this, &ThisClass::ActionName##Pressed); \
	PlayerInputComponent->BindAction(#ActionName, IE_Released, this, &ThisClass::ActionName##Released); \
} \
while (0) \
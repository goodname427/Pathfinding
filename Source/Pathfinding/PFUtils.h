#pragma once

#define UE_LOG_TEMP(Format, ...) UE_LOG(LogTemp, Log, Format, __VA_ARGS__)

#define InitDefaultSubobject(Comp) Comp = CreateDefaultSubobject<std::remove_pointer<decltype(Comp)>::type>(TEXT(#Comp))

#define FastBindAxis(Func) PlayerInputComponent->BindAxis(#Func, this, &ThisClass::##Func)
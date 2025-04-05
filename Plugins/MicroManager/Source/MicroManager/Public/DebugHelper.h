#pragma once

// DebugHelper.h
#pragma once

#include "CoreMinimal.h"
#include "Engine/Engine.h" // Ensure this is included

inline void Print(const FString& Message, const FColor& Color = FColor::Cyan)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 8.0f, Color, Message);
	}
}

inline void PrintLog(const FString& Message)
{
	UE_LOG(LogTemp, Warning, TEXT("%s"), *Message);
}

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "QuicActorActionsWidget.generated.h"

/**
 * 
 */
UCLASS()
class MICROMANAGER_API UQuicActorActionsWidget : public UEditorUtilityWidget
{
	GENERATED_BODY()
private:
	UPROPERTY()
	class UEditorActorSubsystem* EditorActorSubsystem;

	bool GetEditorActorSubsystem();
};

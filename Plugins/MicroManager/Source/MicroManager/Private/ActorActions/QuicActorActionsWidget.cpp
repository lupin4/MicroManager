// Fill out your copyright notice in the Description page of Project Settings.

#include "Subsystems/EditorActorSubsystem.h"
#include "ActorActions/QuicActorActionsWidget.h"

bool UQuicActorActionsWidget::GetEditorActorSubsystem()
{

	GEditor->GetEditorSubsystem<UEditorActorSubsystem>();

	return false;
	
	
}

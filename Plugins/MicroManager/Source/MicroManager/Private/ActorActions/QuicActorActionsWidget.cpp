// Fill out your copyright notice in the Description page of Project Settings.

#include "Subsystems/EditorActorSubsystem.h"
#include "ActorActions/QuicActorActionsWidget.h"
#include "DebugHelper.h"

void UQuicActorActionsWidget::SelectAllActorsWithSimilarName()
{
	if(!GetEditorActorSubsystem()) return;

	TArray<AActor*> SelectedActors = EditorActorSubsystem->GetSelectedLevelActors();
	uint32 SelectionCounter = 0;

	if(SelectedActors.Num()==0)
	{
		DebugHelper::ShowNotifyInfo(TEXT("No actor selected"));
		return;
	}

	if(SelectedActors.Num()>1)
	{
		DebugHelper::ShowNotifyInfo(TEXT("You can only select one actor"));
		return;
	}

	FString SelectedActorName = SelectedActors[0]->GetActorLabel();
	const FString NameToSearch = SelectedActorName.LeftChop(4);

	TArray<AActor*> AllLeveActors = EditorActorSubsystem->GetAllLevelActors();

	for(AActor* ActorInLevel:AllLeveActors)
	{
		if(!ActorInLevel) continue;

		if(ActorInLevel->GetActorLabel().Contains(NameToSearch,SearchCase))
		{
			EditorActorSubsystem->SetActorSelectionState(ActorInLevel,true);
			SelectionCounter++;
		}
	}

	if(SelectionCounter>0)
	{
		DebugHelper::ShowNotifyInfo(TEXT("Successfully selected ") + 
		FString::FromInt(SelectionCounter) + TEXT(" actors"));
	}
	else
	{
		DebugHelper::ShowNotifyInfo(TEXT("No actor with similar name found"));
	}
}


void UQuicActorActionsWidget::DuplicateActors()
{
	if(!GetEditorActorSubsystem()) return;

	TArray<AActor*> SelectedActors = EditorActorSubsystem->GetSelectedLevelActors();
	uint32 Counter = 0;

	if(SelectedActors.Num()==0)
	{
		DebugHelper::ShowNotifyInfo(TEXT("No actor selected"));
		return;
	}

	if(NumberOfDuplicates <=0 || OffsetDistance == 0)
	{
		DebugHelper::ShowNotifyInfo(TEXT("Did not specify a number of duplications or an offset distance"));
		return;
	}

	for(AActor* SelectedActor:SelectedActors)
	{	
		if(!SelectedActor) continue;

		for(int32 i = 0; i<NumberOfDuplicates; i++)
		{
			AActor* DuplicatedActor = 
			EditorActorSubsystem->DuplicateActor(SelectedActor,SelectedActor->GetWorld());

			if(!DuplicatedActor) continue;

			const float DuplicationOffsetDist = (i+1)*OffsetDistance;

			switch(AxisForDuplication)
			{
			case E_DuplicationAxis::EDA_XAxis:

				DuplicatedActor->AddActorWorldOffset(FVector(DuplicationOffsetDist,0.f,0.f));
				break;

			case E_DuplicationAxis::EDA_YAxis:

				DuplicatedActor->AddActorWorldOffset(FVector(0.f,DuplicationOffsetDist,0.f));
				break;

			case E_DuplicationAxis::EDA_ZAxis:

				DuplicatedActor->AddActorWorldOffset(FVector(0.f,0.f,DuplicationOffsetDist));
				break;

			case E_DuplicationAxis::EDA_MAX:
				break;

			default:
				break;
			}

			EditorActorSubsystem->SetActorSelectionState(DuplicatedActor,true);
			Counter++;
		}		
	}

	if(Counter>0)
	{
		DebugHelper::ShowNotifyInfo(TEXT("Successfully duplicated ")+
		FString::FromInt(Counter)+TEXT(" actors"));
	}
}



bool UQuicActorActionsWidget::GetEditorActorSubsystem()
{

	if (!EditorActorSubsystem)
	{
		EditorActorSubsystem = GEditor->GetEditorSubsystem<UEditorActorSubsystem>();
		
	}
	
	return EditorActorSubsystem != nullptr;
	
	
}

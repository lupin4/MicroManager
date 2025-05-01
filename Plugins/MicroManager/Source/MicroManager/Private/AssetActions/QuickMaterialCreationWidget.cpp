// Fill out your copyright notice in the Description page of Project Settings.
#include "AssetActions/QuickMaterialCreationWidget.h"
#include "DebugHelper.h"


#pragma region QuickMaterialCreationCore




#pragma endregion
void UQuickMaterialCreationWidget::CreateMaterialFromSelectedTextures()
{
	if (bCustomMaterialName)
	{
		if (MaterialName.IsEmpty() || MaterialName.Equals(TEXT("M_")))
		{
			DebugHelper::ShowMsgDialog(EAppMsgType::Ok, TEXT("Please enter a valid material name."));
			return;
		}
	}
	//DebugHelper::Print(TEXT("Creating material from selected textures..."), FColor::Cyan);
}

// Fill out your copyright notice in the Description page of Project Settings.



#include "CustomStyle/MicroManagerStyle.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleRegistry.h"

FName FMicroManagerStyle::StyleSetName = FName("MicroManagerStyle");
TSharedPtr<FSlateStyleSet> FMicroManagerStyle::CreatedSlateStyleSet = nullptr;

void FMicroManagerStyle::InitializeIcons()
{	
	if(!CreatedSlateStyleSet.IsValid())
	{
		CreatedSlateStyleSet = CreateSlateStyleSet();
		FSlateStyleRegistry::RegisterSlateStyle(*CreatedSlateStyleSet);
	}
	
}

TSharedRef<FSlateStyleSet> FMicroManagerStyle::CreateSlateStyleSet()
{	
	TSharedRef<FSlateStyleSet> CustomStyleSet = MakeShareable(new FSlateStyleSet(StyleSetName));

	const FString IconDirectory = 
	IPluginManager::Get().FindPlugin(TEXT("MicroManager"))->GetBaseDir() /"Resources";
	
	CustomStyleSet->SetContentRoot(IconDirectory);

	const FVector2D Icon16x16 (16.f,16.f);

	CustomStyleSet->Set("ContentBrowser.DeleteUnusedAssets",
	new FSlateImageBrush(IconDirectory/"DeleteUnusedAsset.png",Icon16x16));

	CustomStyleSet->Set("ContentBrowser.DeleteEmptyFolders",
	new FSlateImageBrush(IconDirectory/"DeleteEmptyFolders.png",Icon16x16));

	CustomStyleSet->Set("ContentBrowser.MicroManager",
	new FSlateImageBrush(IconDirectory/"MicroManager.png",Icon16x16));

	return CustomStyleSet;
}

void FMicroManagerStyle::ShutDown()
{
}

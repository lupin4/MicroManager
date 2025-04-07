// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FMicroManagerModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:


#pragma region ContentBrowserMenuExtension

	// Define the menu extension here.
	void InitCBMenuExtension();

	TArray<FString> FolderPathsSelected;

	TSharedRef<FExtender> CustomBBMenuExtender(const TArray<FString>& SelectedPaths);

	void AddCBMenuEntry(FMenuBuilder& MenuBuilder);

	void OnDeleteUnusedAssetsButtonClicked();

	void FixUpRedirectors();

	void OnDeleteUnusedFoldersButtonClicked();

	void OnMicroManagerClicked();

#pragma endregion

#pragma region CustomEditorTab

	void RegisterMicroManagerTab();

	TSharedRef<SDockTab> OnSpawnMicroManagerTab(const FSpawnTabArgs&);

#pragma endregion
};

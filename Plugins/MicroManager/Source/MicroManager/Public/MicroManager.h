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

	TSharedRef<SDockTab> OnSpawnMicroManagerTab(const FSpawnTabArgs& SpawnTabArgs);

	TArray<TSharedPtr<FAssetData>> GetAllAssetDataUnderSelectedFolders();;





#pragma endregion

public:

#pragma region ProcessDataForMicroManager

	bool DeleteSingleAssetForAssetList(const FAssetData& AssetDataToDelete);
	bool DeleteMultipleAssetsForAssetList(TArray<FAssetData> AssetsToDelete);
	void ListUnusedAssetsForAssetList(const TArray< TSharedPtr <FAssetData> >& AssetsDataToFilter,TArray< TSharedPtr <FAssetData> >& OutUnusedAssetsData);
	void ListSameNameAssetsForAssetList(const TArray< TSharedPtr <FAssetData> >& AssetsDataToFilter,TArray< TSharedPtr <FAssetData> >& OutSameNameAssetsData);
	void SyncCBToClickedAssetForAssetList(const FString& AssetPathsToSync);

	


#pragma endregion	
};

// Copyright Epic Games, Inc. All Rights Reserved.

#include "MicroManager.h"

#include "AssetToolsModule.h"
#include "ContentBrowserModule.h"
#include "DebugHelper.h"
#include "EditorAssetLibrary.h"
#include "ObjectTools.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "SlateWidgets/MicroManagerWidget.h"

#define LOCTEXT_NAMESPACE "FMicroManagerModule"

void FMicroManagerModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	// settings.
    // Register the menu extension.
    InitCBMenuExtension();
	RegisterMicroManagerTab();
}

#pragma region ContentBrowserMenuExtension

// This function registers the menu extension. It will be called when the module is initialized.
void FMicroManagerModule::InitCBMenuExtension()
{
	// Get the Content Browser module.
	FContentBrowserModule& ContentBrowserModule = 
	FModuleManager::LoadModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser"));
	
    // Register a new menu extension and store it in an Array
    // The function returns a reference so it is held in a reference variable.
	// The menu extension will be executed when the Content Browser is opened.
	TArray<FContentBrowserMenuExtender_SelectedPaths>& ContentBrowserModuleMenuExtenders = 
	ContentBrowserModule.GetAllPathViewContextMenuExtenders();


	// Create a new menu item. The menu item will be added to the context menu for paths.
	// Here we are adding a new menu item to the Content Browser's Path View context menu. Using A delegate to define the menu item's behavior.
	
	FContentBrowserMenuExtender_SelectedPaths CustomCBMenuDelegate;

	//Bind the delegate to the function that will be executed when the menu item is clicked.
	// CustomCBMenuDelegate.BindRaw( this, &FMicroManagerModule::CustomBBMenuExtender);
	// ContentBrowserModuleMenuExtenders.Add(CustomCBMenuDelegate);

	ContentBrowserModuleMenuExtenders.Add(FContentBrowserMenuExtender_SelectedPaths::CreateRaw(this, &FMicroManagerModule::CustomBBMenuExtender));
	
}

// First Binding for the menu item.
//Define the position of the menu item in the context menu.
TSharedRef<FExtender> FMicroManagerModule::CustomBBMenuExtender(const TArray<FString>& SelectedPaths)
{
	// Grabbing a TSharedRef of the FExtender.
	TSharedRef<FExtender> MenuExtender(MakeShareable(new FExtender));

	if (SelectedPaths.Num() > 0)
	{
		// Create a new menu item.
		// The menu item will be added to the context menu for paths.
        // Here we are adding a new menu item to the Content Browser's Path View context menu. Using A delegate to define the menu item's behavior.
		MenuExtender->AddMenuExtension(FName("Delete"), // Extension Hook
			EExtensionHook::After, // Hook position
			TSharedPtr<FUICommandList>(), // Custom Hot Keys
			FMenuExtensionDelegate::CreateRaw(this, &FMicroManagerModule::AddCBMenuEntry) // Second binding will determine this menus title, and tooltip
			);
		FolderPathsSelected = SelectedPaths; // Store the selected paths for future use.
	}
	return MenuExtender;
}


// Second Binding for the menu item.
//Define the details of the custom Menu Entry 
void FMicroManagerModule::AddCBMenuEntry(FMenuBuilder& MenuBuilder)
{
	// Top separator
	MenuBuilder.AddMenuSeparator();

	// Delete Unused Assets
	MenuBuilder.AddMenuEntry(
		FText::FromString(TEXT("Delete Unused Assets")),
		FText::FromString(TEXT("Safely delete all unused assets in the directory.")),
		FSlateIcon(),
		FExecuteAction::CreateRaw(this, &FMicroManagerModule::OnDeleteUnusedAssetsButtonClicked)
	);

	// Delete Empty Folders
	MenuBuilder.AddMenuEntry(
		FText::FromString(TEXT("Delete Empty Folders")),
		FText::FromString(TEXT("Safely delete all empty folders in the directory.")),
		FSlateIcon(),
		FExecuteAction::CreateRaw(this, &FMicroManagerModule::OnDeleteUnusedFoldersButtonClicked)
	);

	// Launch Micro Manager
	MenuBuilder.AddMenuEntry(
		FText::FromString(TEXT("Launch Micro Manager")),
		FText::FromString(TEXT("Opens the Micro Manager tool window.")),
		FSlateIcon(),
		FExecuteAction::CreateRaw(this, &FMicroManagerModule::OnMicroManagerClicked)
	);

	// Bottom separator
	MenuBuilder.AddMenuSeparator();
}


// Called when the user clicks the "Delete Unused Assets" menu item.
void FMicroManagerModule::OnDeleteUnusedAssetsButtonClicked()
{
	// DebugHelper::ShowMsgDialog(EAppMsgType::Ok, TEXT("Successfully removed all unused files"));
	if (FolderPathsSelected.Num() > 1)
	{
		DebugHelper::ShowMsgDialog(EAppMsgType::Ok, TEXT("You can only do this with one directory selected."));
		return;
	}
	// DebugHelper::Print(TEXT("Selected folder: ") + FolderPathsSelected[0], FColor::Cyan);

	TArray<FString> AssetPathsNames = UEditorAssetLibrary::ListAssets(FolderPathsSelected[0]);

	if (AssetPathsNames.Num() == 0)
	{
		DebugHelper::ShowMsgDialog(EAppMsgType::Ok, TEXT("No assets found under the selected folder"));
		return;
	}

	EAppReturnType::Type ConfirmedResult =
		DebugHelper::ShowMsgDialog(EAppMsgType::YesNo,
								   FString::Printf(TEXT("A Total of %d Assets to be confirmed for deletion.\nDo you want to delete them?"),
								   AssetPathsNames.Num()), false);

	if (ConfirmedResult == EAppReturnType::No)
	{
		return;
	}
	
	FixUpRedirectors();
	
	TArray<FAssetData> UnusedAssetsData;

	for (const FString& AssetPathName : AssetPathsNames)
	{
		// Don't touch root folder
		//Excludes these folders from the delete process
		if (AssetPathName.Contains(TEXT("Developers")) ||
			AssetPathName.Contains(TEXT("Collections")) ||
			AssetPathName.Contains(TEXT("_ExternalActors_"))||
			AssetPathName.Contains(TEXT("_ExternalObjects_"))||
			AssetPathName.Contains(TEXT("Maps")))
		{
			continue;
		}

		if (!UEditorAssetLibrary::DoesAssetExist(AssetPathName))
		{
			continue;
		}

		TArray<FString> AssetReferencers = UEditorAssetLibrary::FindPackageReferencersForAsset(AssetPathName);

		if (AssetReferencers.Num() == 0)
		{
			const FAssetData UnusedAssetData = UEditorAssetLibrary::FindAssetData(AssetPathName);
			UnusedAssetsData.Add(UnusedAssetData);
		}
	}

	if (UnusedAssetsData.Num() > 0)
	{
		ObjectTools::DeleteAssets(UnusedAssetsData);
	}
	else
	{
		DebugHelper::ShowMsgDialog(EAppMsgType::Ok, TEXT("No unused asset found under the selected folder"), false);
	}
}

/**
 * @brief Deletes all empty folders within the selected directory.
 * 
 * This function identifies and deletes all empty folders within the directory specified by the user.
 * It excludes certain folders from deletion and prompts the user for confirmation before proceeding.
 */
void FMicroManagerModule::OnDeleteUnusedFoldersButtonClicked()
{
    TArray<FString> FolderPathsArray = UEditorAssetLibrary::ListAssets(FolderPathsSelected[0], true, true);
    uint32 Counter = 0;

	// Create a Variable to hold the names of the empty folders
    FString EmptyFolderPathNames;
	// Create a Variable to hold the paths of the empty folders
    TArray<FString> EmptyFoldersPathsArrray;
	// Iterates through all the folders in the selected directory
    for (const FString& FolderPath : FolderPathsArray)
    {
        if (FolderPath.Contains(TEXT("Developers")) ||
            FolderPath.Contains(TEXT("Collections")) ||
            FolderPath.Contains(TEXT("_ExternalActors_"))||
            FolderPath.Contains(TEXT("_ExternalObjects_"))||
            FolderPath.Contains(TEXT("Maps")))
        {
            continue;
        }
        // Checks if the selected path is a directory
        if (!UEditorAssetLibrary::DoesDirectoryExist(FolderPath)) continue;
        if (!UEditorAssetLibrary::DoesDirectoryHaveAssets(FolderPath))
        {
            EmptyFolderPathNames.Append(FolderPath);
            EmptyFolderPathNames.Append(TEXT("\n"));

            EmptyFoldersPathsArrray.Add(FolderPath);
        };
        
    }
    
    if (EmptyFoldersPathsArrray.Num() == 0)
    {
        DebugHelper::ShowMsgDialog(EAppMsgType::Ok, TEXT("No empty folders found under the selected folder"));
        
    }
    EAppReturnType::Type ConfirmedResult =
    DebugHelper::ShowMsgDialog(EAppMsgType::OkCancel, TEXT("Empty Folders found in:\n") + EmptyFolderPathNames + TEXT("\nWould you like to Delete them all?"), false);

    if (ConfirmedResult == EAppReturnType::Cancel) return;

    for (const FString& EmptyFolderPath : EmptyFoldersPathsArrray)
    {
        // Deletes the empty folder recursively.
        //
        UEditorAssetLibrary::DeleteDirectory(EmptyFolderPath)?
            ++Counter : DebugHelper::ShowMsgDialog(EAppMsgType::Ok, TEXT("Failed to delete folder: ") + EmptyFolderPath);
    }

	if (Counter > 0)
	{
		DebugHelper::ShowNotifyInfo(TEXT("Successfully Deleted ") + FString::FromInt(Counter) + TEXT(" Empty Folders"));
	}
        
};

void FMicroManagerModule::OnMicroManagerClicked()
{
	DebugHelper::Print(TEXT("Micro Manager clicked"), FColor::Cyan);
	FGlobalTabmanager::Get()->TryInvokeTab(FName("Micro Manager"));
}

void FMicroManagerModule::FixUpRedirectors()
{

	TArray<UObjectRedirector*> RedirectorsToFixArray;
	FAssetRegistryModule& AssetRegistryModule =
	FModuleManager::Get().LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));

	FARFilter Filter;
	Filter.bRecursivePaths = true;
	Filter.PackagePaths.Emplace("/Game");
	Filter.ClassPaths.Emplace(FTopLevelAssetPath(TEXT("/Script/Engine"), TEXT("ObjectRedirector")));

	TArray<FAssetData> OutRedirectors;
	AssetRegistryModule.Get().GetAssets(Filter,OutRedirectors);

	for(const FAssetData& RedirectorData:OutRedirectors)
	{
		if(UObjectRedirector* RedirectorToFix = Cast<UObjectRedirector>(RedirectorData.GetAsset()))
		{
			RedirectorsToFixArray.Add(RedirectorToFix);
		}
	}

	FAssetToolsModule& AssetToolsModule =
	FModuleManager::LoadModuleChecked<FAssetToolsModule>(TEXT("AssetTools"));

	AssetToolsModule.Get().FixupReferencers(RedirectorsToFixArray);
}

#pragma endregion

#pragma region CustomEditorTab

void FMicroManagerModule::RegisterMicroManagerTab()
{
	// Register the custom editor tab
	FGlobalTabmanager::Get()->RegisterTabSpawner(FName("Micro Manager"),FOnSpawnTab::CreateRaw(this, &FMicroManagerModule::OnSpawnMicroManagerTab)).SetDisplayName(FText::FromString(TEXT("MicroManager")));
}

TSharedRef<SDockTab> FMicroManagerModule::OnSpawnMicroManagerTab(const FSpawnTabArgs& SpawnTabArgs)
{
	return
	SNew(SDockTab).TabRole(ETabRole::NomadTab)
	[
		SNew(SMicroManagerTab)
		.AssetsDataArray(GetAllAssetDataUnderSelectedFolders())
		
	];
}

TArray<TSharedPtr<FAssetData>> FMicroManagerModule::GetAllAssetDataUnderSelectedFolders()
{
	TArray<TSharedPtr<FAssetData>> AvailableAssetsData;

	// Get all asset paths under the selected folder
	if (FolderPathsSelected.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("No folder paths selected. Cannot gather assets."));
		return {}; // empty array to prevent crash
	}

	TArray<FString> AssetPathNames = UEditorAssetLibrary::ListAssets(FolderPathsSelected[0]);

	for (const FString& AssetPath : AssetPathNames)
	{
		// ✅ Check the actual AssetPath here, not the full array
		if (AssetPath.Contains(TEXT("Developers")) ||
			AssetPath.Contains(TEXT("Collections")) ||
			AssetPath.Contains(TEXT("_ExternalActors_")) ||
			AssetPath.Contains(TEXT("_ExternalObjects_")) ||
			AssetPath.Contains(TEXT("Maps")))
		{
			continue;
		}

		if (!UEditorAssetLibrary::DoesAssetExist(AssetPath))
		{
			continue;
		}

		const FAssetData Data = UEditorAssetLibrary::FindAssetData(AssetPath);
		if (Data.IsValid())
		{
			AvailableAssetsData.Add(MakeShared<FAssetData>(Data));
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("Collected %d assets."), AvailableAssetsData.Num());

	return AvailableAssetsData;
}



#pragma endregion



#pragma region ProcessDataForMicromanager


bool FMicroManagerModule::DeleteSingleAssetForAssetList(const FAssetData& AssetDataToDelete)
{
	// Creates an aray to pass data to the delete function
	
	TArray<FAssetData> AssetsDataArrayForDeletion;
	// Adds the asset to be deleted to the array
	AssetsDataArrayForDeletion.Add(AssetDataToDelete);
	// Deletes the asset from the content browser using the ObjectTools module
	if (ObjectTools::DeleteAssets(AssetsDataArrayForDeletion) > 0)
	{
		return true;
	};
	return false;
}

bool FMicroManagerModule::DeleteMultipleAssetsForAssetList(
	const TArray<FAssetData> AssetsToDelete)
{
	if (ObjectTools::DeleteAssets(AssetsToDelete) > 0)
	{
		return true;
		
	}
	return false;
}



void FMicroManagerModule::ListUnusedAssetsForAssetList(
	const TArray<TSharedPtr<FAssetData>>& AssetsDataToFilter, 
	TArray<TSharedPtr<FAssetData>>& OutUnusedAssetsData)
{
	OutUnusedAssetsData.Empty();

	for (const TSharedPtr<FAssetData>& DataSharedPtr : AssetsDataToFilter)
	{
		if (!DataSharedPtr.IsValid())
		{
			continue;
		}

		FString AssetPath = DataSharedPtr->GetObjectPathString();

		TArray<FString> AssetReferencers = 
			UEditorAssetLibrary::FindPackageReferencersForAsset(AssetPath);

		if (AssetReferencers.Num() == 0)
		{
			OutUnusedAssetsData.Add(DataSharedPtr);
		}
	}
}



#pragma endregion



void FMicroManagerModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}



#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FMicroManagerModule, MicroManager)
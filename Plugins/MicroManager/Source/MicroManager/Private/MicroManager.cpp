// Copyright Epic Games, Inc. All Rights Reserved.

#include "MicroManager.h"
#include "ContentBrowserModule.h"

#define LOCTEXT_NAMESPACE "FMicroManagerModule"

void FMicroManagerModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	// settings.

    // Register the menu extension.
    InitCBMenuExtension();
}

// This function registers the menu extension. It will be called when the module is initialized.
#pragma region ContentBrowserMenuExtension
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

TSharedRef<FExtender> FMicroManagerModule::CustomBBMenuExtender(const TArray<FString>& SelectedPaths)
{
	TSharedRef<FExtender> MenuExtender(MakeShareable(new FExtender));

	if (SelectedPaths.Num() > 0)
	{

		// Create a new menu item.
		// The menu item will be added to the context menu for paths.
        // Here we are adding a new menu item to the Content Browser's Path View context menu. Using A delegate to define the menu item's behavior.
		MenuExtender->AddMenuExtension(FName("Delete"),
			EExtensionHook::After,
			TSharedPtr<FUICommandList>(),
			FMenuExtensionDelegate::CreateRaw(this, &FMicroManagerModule::AddCBMenuEntry)
			);
	}
	return MenuExtender;
}


// Second Binding for the menu item.
void FMicroManagerModule::AddCBMenuEntry(FMenuBuilder& MenuBuilder)
{
	MenuBuilder.AddMenuEntry(
		FText::FromString(TEXT("Delete Unused Assets")),
		FText::FromString(TEXT("Safely Delete all unused assets in the directory.")),
		FSlateIcon(),
		FExecuteAction::CreateRaw(this, &FMicroManagerModule::OnDeleteUnusedAssetsButtonClicked)
	);
}


void FMicroManagerModule::OnDeleteUnusedAssetsButtonClicked()
{
    // Your code goes here.
};

#pragma endregion

void FMicroManagerModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}



#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FMicroManagerModule, MicroManager)
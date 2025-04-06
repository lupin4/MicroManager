#include "AssetActions/QuickAssetAction.h"
#include "DebugHelper.h"
#include "EditorUtilityLibrary.h"
#include "EditorAssetLibrary.h"
#include "ObjectTools.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetToolsModule.h"



void UQuickAssetAction::DuplicateAssets(int32 NumOfDuplicates)
{
	if(NumOfDuplicates<=0)
	{
		ShowMsgDialog(EAppMsgType::Ok,TEXT("Please enter a VALID number"));
		return;
	}

	TArray<FAssetData> SelectedAssetsData = UEditorUtilityLibrary::GetSelectedAssetData();
	uint32 Counter = 0;

	for(const FAssetData& SelectedAssetData:SelectedAssetsData)
	{
		for(int32 i = 0; i<NumOfDuplicates; i++)
		{
			const FString SourceAssetPath = SelectedAssetData.GetObjectPathString();
			const FString NewDuplicatedAssetName = SelectedAssetData.AssetName.ToString() + TEXT("_") + FString::FromInt(i+1);
			const FString NewPathName = FPaths::Combine(SelectedAssetData.PackagePath.ToString(),NewDuplicatedAssetName);

			if(UEditorAssetLibrary::DuplicateAsset(SourceAssetPath, NewPathName))
			{	
				UEditorAssetLibrary::SaveAsset(NewPathName,false);
				++Counter;
			}
		}
	}

	if(Counter>0)
	{	
		ShowNotifyInfo(TEXT("Successfully duplicated " + FString::FromInt(Counter) + " files"));
		/*Print(TEXT("Successfully duplicated " + FString::FromInt(Counter) + " files"),FColor::Green);*/
	}
}

void UQuickAssetAction::AddPrefixes()
{
	TArray<UObject*> SelectedObjects = UEditorUtilityLibrary::GetSelectedAssets();
	uint32 Counter = 0;

	for (UObject* SelectedObject : SelectedObjects)
	{
		if (!SelectedObject) continue;

		const FString* PrefixFound = nullptr;
		for (const TPair<UClass*, FString>& Pair : PrefixMap)
		{
			if (SelectedObject->GetClass()->IsChildOf(Pair.Key))
			{
				PrefixFound = &Pair.Value;
				break;
			}
		}

		if (!PrefixFound || PrefixFound->IsEmpty())
		{
			Print(TEXT("No prefix found for " + SelectedObject->GetName()), FColor::Red);
			continue;
		}

		FString OldName = SelectedObject->GetName();
		if (OldName.StartsWith(*PrefixFound))
		{
			Print(TEXT("Name already starts with prefix " + SelectedObject->GetName()), FColor::Yellow);
			continue;
		}
		if (SelectedObject->IsA<UMaterialInstanceConstant>())
		{
			OldName.RemoveFromStart(TEXT("M_"));
			OldName.RemoveFromEnd(TEXT("_Inst"));
			
		}

		const FString NewNameWithPrefix = *PrefixFound + OldName;

		UEditorUtilityLibrary::RenameAsset(SelectedObject, NewNameWithPrefix);
		Counter++;
	}

	ShowNotifyInfo(TEXT("Successfully added prefixes to " + FString::FromInt(Counter) + " files"));
}

/**
 * @brief Removes unused assets from the selected assets in the editor.
 * 
 * This function identifies and deletes assets that have no referencers,
 * effectively cleaning up unused assets from the project.
 * 
 * @param None
 * @return None
 */
void UQuickAssetAction::RemoveUnusedAssets()
{
    TArray<FAssetData> SelectedAssetsData = UEditorUtilityLibrary::GetSelectedAssetData();
    TArray<FAssetData> UnusedAssetsData;
    FixUpRedirectors();

    for (const FAssetData& SelectedAssetData : SelectedAssetsData)
    {
        TArray<FString> AssetReferencers =
        UEditorAssetLibrary::FindPackageReferencersForAsset(SelectedAssetData.ObjectPath.ToString());
        if (AssetReferencers.Num() == 0)
        {
            UnusedAssetsData.Add(SelectedAssetData);
        }
    }
    if (UnusedAssetsData.Num() == 0)
    {
        ShowMsgDialog(EAppMsgType::Ok, TEXT("No unused assets found"));
        return;	
    }
    const int32 NumOfAssetsDeleted = ObjectTools::DeleteAssets(UnusedAssetsData);

    if (NumOfAssetsDeleted == 0)return;
    
    ShowMsgDialog(EAppMsgType::Ok,TEXT("Successfully removed " + FString::FromInt(NumOfAssetsDeleted) + " unused files"));
    
}

/**
 * @brief Fixes up redirectors in the asset registry.
 * 
 * This function searches for all object redirectors within the "/Game" directory
 * and attempts to fix up their referencers. This is useful for cleaning up
 * redirectors that may have been left behind after renaming or moving assets.
 * 
 * @param None
 * @return None
 */
void UQuickAssetAction::FixUpRedirectors()
{
    TArray<UObjectRedirector*> RedirectorsToFixArray;

    FAssetRegistryModule& AssetRegistryModule =
    FModuleManager::Get().LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));

    FARFilter Filter;
    Filter.bRecursivePaths = true;
    Filter.PackagePaths.Emplace("/Game");
    Filter.ClassNames.Emplace("ObjectRedirector");

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
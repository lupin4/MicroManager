// Fill out your copyright notice in the Description page of Project Settings.


#include "AssetActions/QuickAssetAction.h"
#include "DebugHelper.h"
#include "EditorUtilityLibrary.h"
#include "EditorAssetLibrary.h"



void UQuickAssetAction::DuplicateAssets(int32 NumOfDuplicates)
{
	if (NumOfDuplicates <= 0)
	{
		Print(TEXT("Number of duplicates must be greater than 0."), FColor::Red);
		return;
	}

	TArray<FAssetData> SelectedAssetsData = UEditorUtilityLibrary::GetSelectedAssetData();
	uint32 Counter = 0;
	for (const FAssetData& SelectedAssetData : SelectedAssetsData)
	{
		for (int32 i = 0; i < NumOfDuplicates; ++i)
		{
			// Get the source asset path and create a new asset name with an incrementing number.
			const FString SourceAssetPath = SelectedAssetData.GetObjectPathString();
			// Get the new asset name by appending an incrementing number to the original asset name. 
			const FString NewDuplicateAssetName = SelectedAssetData.AssetName.ToString() + TEXT("_") + FString::FromInt(i+1);
			// Construct the new asset path by combining the package path and the new asset name.
			const FString NewPathName = FPaths::Combine(*SelectedAssetData.PackagePath.ToString(), NewDuplicateAssetName);
			// Call the duplicate asset function  in the UEditorAssetLibrary and save the new asset.
			if (UEditorAssetLibrary::DuplicateAsset(SourceAssetPath, NewPathName))
			{
				UEditorAssetLibrary::SaveAsset(NewPathName, false);

				
                ++Counter;
				
			}
			
		}
	}

	if (Counter > 0)
	{
		Print(FString::Printf(TEXT("Successfully Duplicated %d Files"), Counter), FColor::Green);

		
	}
	
}

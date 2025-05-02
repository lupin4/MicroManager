// Fill out your copyright notice in the Description page of Project Settings.
#include "AssetActions/QuickMaterialCreationWidget.h"
#include "EditorUtilityLibrary.h"
#include "DebugHelper.h"


#pragma region QuickMaterialCreationCore

void UQuickMaterialCreationWidget::CreateMaterialFromSelectedTextures()
{
	if (bCustomMaterialName)
	{
		if (MaterialName.IsEmpty() || MaterialName.Equals(TEXT("M_")))
		{
			DebugHelper::ShowMsgDialog(EAppMsgType::Ok, TEXT("Please enter a valid material name."));
			return;
		}

		TArray<FAssetData> SelectedAssetData = UEditorUtilityLibrary::GetSelectedAssetData();
		TArray<UTexture2D*> SelectedTexturesArray;
		FString SelectedTextureFolderPath;
		if (ProcessSelectedData(SelectedAssetData, SelectedTexturesArray, SelectedTextureFolderPath)) return;
	}
	//DebugHelper::Print(TEXT("Creating material from selected textures..."), FColor::Cyan);
}


#pragma endregion

#pragma region QuickMaterialCreation
bool UQuickMaterialCreationWidget::ProcessSelectedData(const TArray<FAssetData>& SelectedDataToProccess, 
TArray<UTexture2D*>& OutSelectedTexturesArray, FString& OutSelectedTexturePackagePath)
{
	if(SelectedDataToProccess.Num()==0)
	{
		DebugHelper::ShowMsgDialog(EAppMsgType::Ok,TEXT("No texture selected"));
		return false;
	}

	bool bMaterialNameSet = false;

	for(const FAssetData& SelectedData:SelectedDataToProccess)
	{
		UObject* SelectedAsset = SelectedData.GetAsset();

		if(!SelectedAsset) continue;

		UTexture2D* SelectedTexture = Cast<UTexture2D>(SelectedAsset);

		if(!SelectedTexture)
		{
			DebugHelper::ShowMsgDialog(EAppMsgType::Ok,TEXT("Please select only textures\n") + 
			SelectedAsset->GetName() + TEXT(" is not a texture"));

			return false;
		}

		OutSelectedTexturesArray.Add(SelectedTexture);

		if(OutSelectedTexturePackagePath.IsEmpty())
		{
			OutSelectedTexturePackagePath = SelectedData.PackagePath.ToString();
		}

		if(!bCustomMaterialName && !bMaterialNameSet)
		{
			MaterialName = SelectedAsset->GetName();
			MaterialName.RemoveFromStart(TEXT("T_"));
			MaterialName.InsertAt(0,TEXT("M_"));

			bMaterialNameSet = true;
		}		
	}

	return true;
}
#pragma endregion
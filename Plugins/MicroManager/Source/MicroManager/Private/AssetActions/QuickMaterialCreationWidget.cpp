
#include "AssetActions/QuickMaterialCreationWidget.h"
#include "DebugHelper.h"
#include "EditorUtilityLibrary.h"
#include "EditorAssetLibrary.h"
#include "AssetToolsModule.h"
#include "Factories/MaterialFactoryNew.h"
#include "Materials/MaterialExpressionTextureSample.h"

#pragma region QuickMaterialCreationCore

void UQuickMaterialCreationWidget::CreateMaterialFromSelectedTextures()
{
	if (bCustomMaterialName)
	{
		if (MaterialName.IsEmpty() || MaterialName.Equals(TEXT("M_")))
		{
			DebugHelper::ShowMsgDialog(EAppMsgType::Ok, TEXT("Please enter a valid name"));
			return;
		}
	}

	TArray<FAssetData> SelectedAssetsData = UEditorUtilityLibrary::GetSelectedAssetData();
	TArray<UTexture2D*> SelectedTexturesArray;
	FString SelectedTextureFolderPath;
	uint32 PinsConnectedCounter = 0;

	if (!ProcessSelectedData(SelectedAssetsData, SelectedTexturesArray, SelectedTextureFolderPath)) return;
	if (CheckIsNameUsed(SelectedTextureFolderPath, MaterialName)) return;

	UMaterial* CreatedMaterial = CreateMaterialAsset(MaterialName, SelectedTextureFolderPath);
	if (!CreatedMaterial)
	{
		DebugHelper::ShowMsgDialog(EAppMsgType::Ok, TEXT("Failed to create material"));
		return;
	}

	for (UTexture2D* SelectedTexture : SelectedTexturesArray)
	{
		if (!SelectedTexture) continue;
		Default_CreateMaterialNodes(CreatedMaterial, SelectedTexture, PinsConnectedCounter);
	}
}

bool UQuickMaterialCreationWidget::ProcessSelectedData(const TArray<FAssetData>& SelectedDataToProccess,
	TArray<UTexture2D*>& OutSelectedTexturesArray, FString& OutSelectedTexturePackagePath)
{
	if (SelectedDataToProccess.Num() == 0)
	{
		DebugHelper::ShowMsgDialog(EAppMsgType::Ok, TEXT("No texture selected"));
		return false;
	}

	bool bMaterialNameSet = false;

	for (const FAssetData& SelectedData : SelectedDataToProccess)
	{
		UObject* SelectedAsset = SelectedData.GetAsset();
		if (!SelectedAsset) continue;

		UTexture2D* SelectedTexture = Cast<UTexture2D>(SelectedAsset);
		if (!SelectedTexture)
		{
			DebugHelper::ShowMsgDialog(EAppMsgType::Ok, TEXT("Please select only textures") + SelectedAsset->GetName() + TEXT(" is not a texture"));
			return false;
		}

		OutSelectedTexturesArray.Add(SelectedTexture);
		if (OutSelectedTexturePackagePath.IsEmpty())
		{
			OutSelectedTexturePackagePath = SelectedData.PackagePath.ToString();
		}

		if (!bCustomMaterialName && !bMaterialNameSet)
		{
			MaterialName = SelectedAsset->GetName();
			MaterialName.RemoveFromStart(TEXT("T_"));
			MaterialName.InsertAt(0, TEXT("M_"));
			bMaterialNameSet = true;
		}
	}
	return true;
}

bool UQuickMaterialCreationWidget::CheckIsNameUsed(const FString& FolderPathToCheck,
	const FString& MaterialNameToCheck)
{
	TArray<FString> ExistingAssetsPaths = UEditorAssetLibrary::ListAssets(FolderPathToCheck, false);
	for (const FString& ExistingAssetPath : ExistingAssetsPaths)
	{
		const FString ExistingAssetName = FPaths::GetBaseFilename(ExistingAssetPath);
		if (ExistingAssetName.Equals(MaterialNameToCheck))
		{
			DebugHelper::ShowMsgDialog(EAppMsgType::Ok, MaterialNameToCheck + TEXT(" is already used by asset"));
			return true;
		}
	}
	return false;
}

UMaterial* UQuickMaterialCreationWidget::CreateMaterialAsset(const FString& NameOfTheMaterial, const FString& PathToPutMaterial)
{
	FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>(TEXT("AssetTools"));
	UMaterialFactoryNew* MaterialFactory = NewObject<UMaterialFactoryNew>();
	UObject* CreatedObject = AssetToolsModule.Get().CreateAsset(NameOfTheMaterial, PathToPutMaterial,
		UMaterial::StaticClass(), MaterialFactory);
	return Cast<UMaterial>(CreatedObject);
}

void UQuickMaterialCreationWidget::Default_CreateMaterialNodes(UMaterial* CreatedMaterial,
	UTexture2D* SelectedTexture, uint32& PinsConnectedCounter)
{
	UMaterialExpressionTextureSample* TextureSampleNode = NewObject<UMaterialExpressionTextureSample>(CreatedMaterial);
	if (!TextureSampleNode) return;

	if (!CreatedMaterial->GetEditorOnlyData()->BaseColor.IsConnected())
	{
		if (TryConnectBaseColor(TextureSampleNode, SelectedTexture, CreatedMaterial))
		{
			PinsConnectedCounter++;
			return;
		}
	}
}

#pragma endregion

#pragma region CreateMaterialNodes

bool UQuickMaterialCreationWidget::TryConnectBaseColor(UMaterialExpressionTextureSample* TextureSampleNode,
	UTexture2D* SelectedTexture, UMaterial* CreatedMaterial)
{
	for (const FString& BaseColorName : BaseColorArray)
	{
		if (SelectedTexture->GetName().Contains(BaseColorName))
		{
			TextureSampleNode->Texture = SelectedTexture;

			CreatedMaterial->GetEditorOnlyData()->ExpressionCollection.Expressions.Add(TextureSampleNode);
			CreatedMaterial->GetEditorOnlyData()->BaseColor.Expression = TextureSampleNode;
			CreatedMaterial->PostEditChange();

			TextureSampleNode->MaterialExpressionEditorX -= 600;
			return true;
		}
	}
	return false;
}

#pragma endregion

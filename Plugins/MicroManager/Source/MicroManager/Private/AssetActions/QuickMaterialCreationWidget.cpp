// Fill out your copyright notice in the Description page of Project Settings.


#include "AssetActions/QuickMaterialCreationWidget.h"
#include "DebugHelper.h"
#include "EditorUtilityLibrary.h"
#include "EditorAssetLibrary.h"
#include "AssetToolsModule.h"
#include "Factories/MaterialFactoryNew.h"
#include "Materials/MaterialInstanceConstant.h"
#include "Factories/MaterialInstanceConstantFactoryNew.h"
#include "Materials/MaterialExpressionMultiply.h"
#include "Materials/MaterialExpressionTextureSampleParameter2D.h"
#include "Materials/MaterialExpressionVectorParameter.h"
#include "Factories/MaterialInstanceConstantFactoryNew.h"
#include "EditorAssetLibrary.h"
#include "AssetToolsModule.h"
#include "EditorUtilityLibrary.h"
#include "AssetRegistry/AssetRegistryModule.h"


#pragma region QuickMaterialCreationCore
	
void UQuickMaterialCreationWidget::CreateMaterialFromSelectedTextures()
{
	if(bCustomMaterialName)
	{
		if(MaterialName.IsEmpty() || MaterialName.Equals(TEXT("M_")))
		{
			DebugHelper::ShowMsgDialog(EAppMsgType::Ok,TEXT("Please enter a valid name"));
			return;
		}
	}

	TArray<FAssetData> SelectedAssetsData = UEditorUtilityLibrary::GetSelectedAssetData();
	TArray<UTexture2D*> SelectedTexturesArray;
	FString SelectedTextureFolderPath;
	uint32 PinsConnectedCounter = 0;

	if(!ProcessSelectedData(SelectedAssetsData, SelectedTexturesArray, SelectedTextureFolderPath)) {MaterialName = TEXT("M_"); return;}
	
	if(CheckIsNameUsed(SelectedTextureFolderPath, MaterialName)) {MaterialName = TEXT("M_"); return;}

	UMaterial* CreatedMaterial = CreateMaterialAsset(MaterialName,SelectedTextureFolderPath);

	if(!CreatedMaterial)
	{
		DebugHelper::ShowMsgDialog(EAppMsgType::Ok,TEXT("Failed to create material"));
		return;
	}

	for(UTexture2D* SelectedTexture:SelectedTexturesArray)
	{
		if(!SelectedTexture) continue;

		Default_CreateMaterialNodes(CreatedMaterial,SelectedTexture,PinsConnectedCounter);
	}

	if(PinsConnectedCounter>0)
	{
		DebugHelper::ShowNotifyInfo(TEXT("Successfully connected ") 
		+ FString::FromInt(PinsConnectedCounter) + (TEXT(" pins")));
	}
	

	if (bCreateMaterialInstance)
	{
		CreateMaterialInstanceAsset(CreatedMaterial, MaterialName, SelectedTextureFolderPath);
	}
	MaterialName = TEXT("M_");
}

//Process the selected data, will filter out textures,and return false if non-texture selected
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

//Will return true if the material name is used by asset under the specified folder
bool UQuickMaterialCreationWidget::CheckIsNameUsed(const FString& FolderPathToCheck, 
const FString& MaterialNameToCheck)
{
	TArray<FString> ExistingAssetsPaths = UEditorAssetLibrary::ListAssets(FolderPathToCheck,false);

	for(const FString& ExistingAssetPath:ExistingAssetsPaths)
	{
		const FString ExistingAssetName = FPaths::GetBaseFilename(ExistingAssetPath);

		if(ExistingAssetName.Equals(MaterialNameToCheck))
		{
			DebugHelper::ShowMsgDialog(EAppMsgType::Ok,MaterialNameToCheck + 
			TEXT(" is already used by asset"));

			return true;
		}
	}

	return false;

}

UMaterial * UQuickMaterialCreationWidget::CreateMaterialAsset(const FString & NameOfTheMaterial, const FString & PathToPutMaterial)
{	
	FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>(TEXT("AssetTools"));

	UMaterialFactoryNew* MaterialFactory = NewObject<UMaterialFactoryNew>();

	UObject* CreatedObject = AssetToolsModule.Get().CreateAsset(NameOfTheMaterial,PathToPutMaterial,
	UMaterial::StaticClass(),MaterialFactory);

	return Cast<UMaterial>(CreatedObject);
}

void UQuickMaterialCreationWidget::Default_CreateMaterialNodes(UMaterial* CreatedMaterial, 
UTexture2D * SelectedTexture, uint32 & PinsConnectedCounter)
{
	UMaterialExpressionTextureSample* TextureSampleNode =
	NewObject<UMaterialExpressionTextureSample>(CreatedMaterial);

	if(!TextureSampleNode) return;

	if (!CreatedMaterial->GetEditorOnlyData()->BaseColor.IsConnected())
	{
		if (bUseMaterialParameters)
		{
			// Create Texture Parameter node
			UMaterialExpressionTextureSampleParameter2D* TextureParam =
				NewObject<UMaterialExpressionTextureSampleParameter2D>(CreatedMaterial);
			TextureParam->ParameterName = BaseColorParameterName;
			TextureParam->Texture = SelectedTexture;
			TextureParam->SamplerType = SAMPLERTYPE_Color;
			TextureParam->MaterialExpressionEditorX = -800;
			TextureParam->MaterialExpressionEditorY = 0;

			// Create Vector Tint Parameter node
			UMaterialExpressionVectorParameter* TintParam =
				NewObject<UMaterialExpressionVectorParameter>(CreatedMaterial);
			TintParam->ParameterName = TEXT("BaseColorTint");
			TintParam->DefaultValue = BaseColorTint;
			TintParam->MaterialExpressionEditorX = -800;
			TintParam->MaterialExpressionEditorY = -200;

			// Multiply Texture * Tint
			UMaterialExpressionMultiply* MultiplyNode =
				NewObject<UMaterialExpressionMultiply>(CreatedMaterial);
			MultiplyNode->MaterialExpressionEditorX = -400;
			MultiplyNode->MaterialExpressionEditorY = -100;
			MultiplyNode->A.Connect(0, TextureParam);
			MultiplyNode->B.Connect(0, TintParam);

			auto& Expressions = CreatedMaterial->GetEditorOnlyData()->ExpressionCollection.Expressions;
			Expressions.Add(TextureParam);
			Expressions.Add(TintParam);
			Expressions.Add(MultiplyNode);

			CreatedMaterial->GetEditorOnlyData()->BaseColor.Expression = MultiplyNode;
			CreatedMaterial->PostEditChange();

			PinsConnectedCounter++;
			return;
		}
		else
		{
			if (TryConnectBaseColor(TextureSampleNode, SelectedTexture, CreatedMaterial))
			{
				PinsConnectedCounter++;
				return;
			}
		}
	}


	if(!CreatedMaterial->GetEditorOnlyData()->Metallic.IsConnected())
	{
		if(TryConnectMetalic(TextureSampleNode, SelectedTexture, CreatedMaterial))
		{
			PinsConnectedCounter++;
			return;
		}
	}
	
	if(!CreatedMaterial->GetEditorOnlyData()->Roughness.IsConnected())
	{
		if(TryConnectRoughness(TextureSampleNode, SelectedTexture, CreatedMaterial))
		{
			PinsConnectedCounter++;
			return;
		}
	}

	if(!CreatedMaterial->GetEditorOnlyData()->Normal.IsConnected())
	{
		if(TryConnectNormal(TextureSampleNode, SelectedTexture, CreatedMaterial))
		{
			PinsConnectedCounter++;
			return;
		}
	}

	if(!CreatedMaterial->GetEditorOnlyData()->AmbientOcclusion.IsConnected())
	{
		if(TryConnectAO(TextureSampleNode, SelectedTexture, CreatedMaterial))
		{
			PinsConnectedCounter++;
			return;
		}
	}

	if (!CreatedMaterial->GetEditorOnlyData()->Metallic.IsConnected() &&
	!CreatedMaterial->GetEditorOnlyData()->Roughness.IsConnected() &&
	!CreatedMaterial->GetEditorOnlyData()->AmbientOcclusion.IsConnected())
	{
		if (TryConnectORM(TextureSampleNode, SelectedTexture, CreatedMaterial))
		{
			PinsConnectedCounter += 3; // because we connect 3 pins at once
			return;
		}
	}


	DebugHelper::Print(TEXT("Failed to connect the texture: ") + SelectedTexture->GetName(),FColor::Red);
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

bool UQuickMaterialCreationWidget::TryConnectMetalic(UMaterialExpressionTextureSample * TextureSampleNode, 
UTexture2D * SelectedTexture, UMaterial * CreatedMaterial)
{
	for(const FString& MetalicName:MetallicArray)
	{
		if(SelectedTexture->GetName().Contains(MetalicName))
		{
			SelectedTexture->CompressionSettings = TextureCompressionSettings::TC_Default;
			SelectedTexture->SRGB = false;
			SelectedTexture->PostEditChange();

			TextureSampleNode->Texture = SelectedTexture;
			TextureSampleNode->SamplerType = EMaterialSamplerType::SAMPLERTYPE_LinearColor;

			CreatedMaterial->GetEditorOnlyData()->ExpressionCollection.Expressions.Add(TextureSampleNode);
			CreatedMaterial->GetEditorOnlyData()->Metallic.Expression = TextureSampleNode;
			CreatedMaterial->PostEditChange();

			TextureSampleNode->MaterialExpressionEditorX -=600;
			TextureSampleNode->MaterialExpressionEditorY +=240;

			return true;
		}
	}
	
	return false;
}



bool UQuickMaterialCreationWidget::TryConnectRoughness(UMaterialExpressionTextureSample * TextureSampleNode, UTexture2D * SelectedTexture, UMaterial * CreatedMaterial)
{
	for(const FString& RoughnessName:RoughnessArray)
	{
		if(SelectedTexture->GetName().Contains(RoughnessName))
		{
			SelectedTexture->CompressionSettings = TextureCompressionSettings::TC_Default;
			SelectedTexture->SRGB = false;
			SelectedTexture->PostEditChange();

			TextureSampleNode->Texture = SelectedTexture;
			TextureSampleNode->SamplerType = EMaterialSamplerType::SAMPLERTYPE_LinearColor;

			CreatedMaterial->GetEditorOnlyData()->ExpressionCollection.Expressions.Add(TextureSampleNode);
			CreatedMaterial->GetEditorOnlyData()->Roughness.Expression = TextureSampleNode;
			CreatedMaterial->PostEditChange();

			TextureSampleNode->MaterialExpressionEditorX -=600;
			TextureSampleNode->MaterialExpressionEditorY +=480;

			return true;
		}
	}

	return false;
}

bool UQuickMaterialCreationWidget::TryConnectNormal(UMaterialExpressionTextureSample * TextureSampleNode, UTexture2D * SelectedTexture, UMaterial * CreatedMaterial)
{
	for(const FString& NormalName:NormalArray)
	{	
		if(SelectedTexture->GetName().Contains(NormalName))
		{
			TextureSampleNode->Texture = SelectedTexture;
			TextureSampleNode->SamplerType = EMaterialSamplerType::SAMPLERTYPE_Normal;

			CreatedMaterial->GetEditorOnlyData()->ExpressionCollection.Expressions.Add(TextureSampleNode);
			CreatedMaterial->GetEditorOnlyData()->Normal.Expression = TextureSampleNode;
			CreatedMaterial->PostEditChange();

			TextureSampleNode->MaterialExpressionEditorX -= 600;
			TextureSampleNode->MaterialExpressionEditorY += 720;

			return true;
		}
	}
	
	return false;
}

bool UQuickMaterialCreationWidget::TryConnectAO(UMaterialExpressionTextureSample * TextureSampleNode, UTexture2D * SelectedTexture, UMaterial * CreatedMaterial)
{
	for(const FString& AOName:AmbientOcclusionArray)
	{	
		if(SelectedTexture->GetName().Contains(AOName))
		{
			SelectedTexture->CompressionSettings = TextureCompressionSettings::TC_Default;
			SelectedTexture->SRGB = false;
			SelectedTexture->PostEditChange();

			TextureSampleNode->Texture = SelectedTexture;
			TextureSampleNode->SamplerType = EMaterialSamplerType::SAMPLERTYPE_LinearColor;

			CreatedMaterial->GetEditorOnlyData()->ExpressionCollection.Expressions.Add(TextureSampleNode);
			CreatedMaterial->GetEditorOnlyData()->AmbientOcclusion.Expression = TextureSampleNode;
			CreatedMaterial->PostEditChange();


			TextureSampleNode->MaterialExpressionEditorX -= 600;
			TextureSampleNode->MaterialExpressionEditorY += 960;

			return true;
		}
	}

	return false;

}
bool UQuickMaterialCreationWidget::TryConnectORM(UMaterialExpressionTextureSample * TextureSampleNode, UTexture2D * SelectedTexture, UMaterial * CreatedMaterial)
{	
	for(const FString& ORM_Name:ORMArray)
	{
		if(SelectedTexture->GetName().Contains(ORM_Name))
		{
			SelectedTexture->CompressionSettings = TC_Masks;
			SelectedTexture->SRGB = false;
			SelectedTexture->PostEditChange();

			TextureSampleNode->Texture = SelectedTexture;
			TextureSampleNode->SamplerType = SAMPLERTYPE_Masks;

			CreatedMaterial->GetEditorOnlyData()->ExpressionCollection.Expressions.Add(TextureSampleNode);
			CreatedMaterial->GetEditorOnlyData()->AmbientOcclusion.Connect(0, TextureSampleNode); // Red
			CreatedMaterial->GetEditorOnlyData()->Roughness.Connect(1, TextureSampleNode);        // Green
			CreatedMaterial->GetEditorOnlyData()->Metallic.Connect(2, TextureSampleNode);         // Blue

			CreatedMaterial->PostEditChange();

			TextureSampleNode->MaterialExpressionEditorX -= 600;
			TextureSampleNode->MaterialExpressionEditorY += 960;

			return true;
		}
	}
	return false;
}

#pragma endregion

UMaterialInstanceConstant* UQuickMaterialCreationWidget::CreateMaterialInstanceAsset(
	UMaterial* CreatedMaterial, FString MaterialInstanceName, const FString& PathToPutMI)
{
	MaterialInstanceName.RemoveFromStart(TEXT("M_"));
	MaterialInstanceName.InsertAt(0, TEXT("MI_"));

	UMaterialInstanceConstantFactoryNew* MIFactoryNew = NewObject<UMaterialInstanceConstantFactoryNew>();

	FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>(TEXT("AssetTools"));

	UObject* CreatedObject = AssetToolsModule.Get().CreateAsset(
		MaterialInstanceName,
		PathToPutMI,
		UMaterialInstanceConstant::StaticClass(),
		MIFactoryNew);

	if (UMaterialInstanceConstant* CreatedMI = Cast<UMaterialInstanceConstant>(CreatedObject))
	{
		CreatedMI->SetParentEditorOnly(CreatedMaterial);
		CreatedMI->PostEditChange();
		CreatedMaterial->PostEditChange();
		return CreatedMI;
	}

	return nullptr;
}

void UQuickMaterialCreationWidget::CreateMaterialInstanceFromSelectedMaterial()
{
	TArray<FAssetData> SelectedAssets = UEditorUtilityLibrary::GetSelectedAssetData();
	uint32 NumCreated = 0;

	for (const FAssetData& AssetData : SelectedAssets)
	{
		UMaterial* Material = Cast<UMaterial>(AssetData.GetAsset());
		if (!Material)
		{
			DebugHelper::Print(TEXT("Selected asset is not a Material"), FColor::Red);
			continue;
		}

		// Set up factory
		UMaterialInstanceConstantFactoryNew* MICFactory = NewObject<UMaterialInstanceConstantFactoryNew>();
		MICFactory->InitialParent = Material;

		// Create asset path and name
		const FString AssetName = FString::Printf(TEXT("MI_%s"), *Material->GetName().Replace(TEXT("M_"), TEXT("")));
		const FString PackagePath = AssetData.PackagePath.ToString();
		const FString AssetPath = FPaths::Combine(PackagePath, AssetName);

		// Check if asset already exists
		if (UEditorAssetLibrary::DoesAssetExist(AssetPath))
		{
			DebugHelper::Print(FString::Printf(TEXT("Material instance already exists: %s"), *AssetName), FColor::Yellow);
			continue;
		}

		// Use AssetTools to create new asset
		FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools");
		UObject* CreatedAsset = AssetToolsModule.Get().CreateAsset(AssetName, PackagePath, UMaterialInstanceConstant::StaticClass(), MICFactory);

		if (CreatedAsset)
		{
			UEditorAssetLibrary::SaveAsset(CreatedAsset->GetPathName(), false);
			DebugHelper::PrintLog(FString::Printf(TEXT("Created Material Instance: %s"), *CreatedAsset->GetPathName()));
			++NumCreated;
		}
	}

	if (NumCreated > 0)
	{
		DebugHelper::ShowNotifyInfo(FString::Printf(TEXT("Created %d Material Instance(s)"), NumCreated));
	}
	else
	{
		DebugHelper::ShowMsgDialog(EAppMsgType::Ok, TEXT("No Material Instances were created."));
	}
}



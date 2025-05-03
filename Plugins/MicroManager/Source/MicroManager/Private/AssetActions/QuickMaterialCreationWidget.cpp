// Fill out your copyright notice in the Description page of Project Settings.


#include "AssetActions/QuickMaterialCreationWidget.h"
#include "DebugHelper.h"
#include "EditorUtilityLibrary.h"
#include "EditorAssetLibrary.h"
#include "AssetToolsModule.h"
#include "Factories/MaterialFactoryNew.h"

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

	if(!ProcessSelectedData(SelectedAssetsData, SelectedTexturesArray, SelectedTextureFolderPath)) return;
	
	if(CheckIsNameUsed(SelectedTextureFolderPath, MaterialName)) return;

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

	if(!CreatedMaterial->GetEditorOnlyData()->BaseColor.IsConnected())
	{
		if(TryConnectBaseColor(TextureSampleNode,SelectedTexture,CreatedMaterial))
		{
			PinsConnectedCounter++;
			return;
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

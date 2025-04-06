// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AssetActionUtility.h"
#include "Materials/Material.h"
#include "Materials/MaterialInstanceConstant.h"
#include "Particles/ParticleSystem.h"	
#include "Sound/SoundCue.h"
#include "Sound/SoundWave.h"
#include "Engine/Texture.h"
#include "Engine/Texture2D.h"
#include "Engine/TextureCube.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Animation/AnimBlueprint.h"
#include "Animation/AnimSequence.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimComposite.h"
#include "Animation/BlendSpace.h"
#include "Animation/AimOffsetBlendSpace.h"
#include "Animation/AnimStateMachineTypes.h"
#include "Camera/CameraAnimationHelper.h"
#include "Camera/CameraActor.h"
#include "Camera/CameraShakeBase.h"
#include "Camera/CameraComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/SkeletalMesh.h"
#include "Engine/Font.h"
#include "Curves/CurveFloat.h"
#include "Curves/CurveVector.h"
#include "Curves/CurveLinearColor.h"
#include "Engine/Light.h"
#include "Engine/DataTable.h"
#include "Engine/PrimaryAssetLabel.h"
#include "Engine/DataAsset.h"
#include "Engine/World.h"
#include "GameFramework/Volume.h"
#include "Engine/StaticMeshActor.h"
#include "LevelSequence.h"
#include "Blueprint/UserWidget.h"
#include "NiagaraSystem.h"
#include "NiagaraEmitter.h"
#include "PhysicsEngine/PhysicsAsset.h"
#include "Runtime/LevelSequence/Public/LevelSequence.h"
#include "QuickAssetAction.generated.h"

/**
 * 
 */
UCLASS()
// This is a custom asset action that can be used to perform a specific action on selected assets.
//It is Derived from UAssetActionUtility and must implement the required functions.	

class MICROMANAGER_API UQuickAssetAction : public UAssetActionUtility
{
	GENERATED_BODY()
public:
	// Begin UAssetActionUtility
	UFUNCTION(CallInEditor)
	void DuplicateAssets(int32 NumOfDuplicates);

	UFUNCTION(CallInEditor)
	void AddPrefixes();

	UFUNCTION(CallInEditor)
	void RemoveUnusedAssets();
private:
	TMap<UClass*, FString> PrefixMap =
{
    // Blueprints & Widgets
    { UBlueprint::StaticClass(), TEXT("BP_") },
    { UAnimBlueprint::StaticClass(), TEXT("ABP_") },
    { UUserWidget::StaticClass(), TEXT("WBP_") },

    // Meshes
    { UStaticMesh::StaticClass(), TEXT("SM_") },
    { USkeletalMesh::StaticClass(), TEXT("SK_") },

    // Materials
    { UMaterial::StaticClass(), TEXT("M_") },
    { UMaterialInstanceConstant::StaticClass(), TEXT("MI_") },
    { UMaterialFunctionInterface::StaticClass(), TEXT("MF_") },

    // Textures
    { UTexture2D::StaticClass(), TEXT("T_") },
    { UTextureCube::StaticClass(), TEXT("T_") },
    { UTextureRenderTarget2D::StaticClass(), TEXT("RT_") },

    // Sounds
    { USoundCue::StaticClass(), TEXT("SC_") },
    { USoundWave::StaticClass(), TEXT("SW_") },

    // Effects
    { UParticleSystem::StaticClass(), TEXT("PS_") },
    { UNiagaraSystem::StaticClass(), TEXT("NS_") },
    { UNiagaraEmitter::StaticClass(), TEXT("NE_") },

    // Animation
    { UAnimSequence::StaticClass(), TEXT("A_") },
    { UAnimMontage::StaticClass(), TEXT("AM_") },
    { UAnimComposite::StaticClass(), TEXT("AC_") },
    { UBlendSpace::StaticClass(), TEXT("BS_") },
    { UAimOffsetBlendSpace::StaticClass(), TEXT("AO_") },

    // Skeleton & Physics
    { USkeleton::StaticClass(), TEXT("SKEL_") },
    { UPhysicsAsset::StaticClass(), TEXT("PHYS_") },

    // Fonts & Curves
    { UFont::StaticClass(), TEXT("Font_") },
    { UCurveFloat::StaticClass(), TEXT("Curve_") },
    { UCurveVector::StaticClass(), TEXT("Curve_") },
    { UCurveLinearColor::StaticClass(), TEXT("Curve_") },

    // Data Assets
    { UDataTable::StaticClass(), TEXT("DT_") },
    { UDataAsset::StaticClass(), TEXT("DA_") },
    { UPrimaryDataAsset::StaticClass(), TEXT("DA_") },

    // Cinematics / Cameras
    { ULevelSequence::StaticClass(), TEXT("Seq_") },
    { UCameraShakeBase::StaticClass(), TEXT("CS_") },
    { UCameraComponent::StaticClass(), TEXT("CC_") },

    // World / Level
    { UWorld::StaticClass(), TEXT("Lvl_") },
    { AVolume::StaticClass(), TEXT("Vol_") },
    { ALight::StaticClass(), TEXT("Light_") },
    { AStaticMeshActor::StaticClass(), TEXT("SMA_") },
};
	void FixUpRedirectors();

	
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AssetActionUtility.h"
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
	
};

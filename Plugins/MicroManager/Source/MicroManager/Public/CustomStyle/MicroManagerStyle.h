// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Styling/SlateStyleRegistry.h"
#include "Styling/SlateStyle.h"
#include "Styling/SlateTypes.h"
#include "Styling/SlateBrush.h"
#include "Framework/Application/SlateApplication.h"


class FMicroManagerStyle
{
public:
	static void InitializeIcons();
	static void ShutDown();

private:
	static FName StyleSetName;

	static TSharedRef<FSlateStyleSet> CreateSlateStyleSet();
	static TSharedPtr<FSlateStyleSet> CreatedSlateStyleSet;

public:
	static FName GetStyleSetName() { return StyleSetName; }
};




// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Widgets/SCompoundWidget.h"

/**
 * SMicroManagerTab
 * Custom Slate UI widget that displays a list of assets with checkboxes.
 */
class SMicroManagerTab : public SCompoundWidget
{
	SLATE_BEGIN_ARGS(SMicroManagerTab) {}
	// Defines a single argument to initialize the asset list
	SLATE_ARGUMENT(TArray<TSharedPtr<FAssetData>>, AssetsDataArray)
SLATE_END_ARGS()

public:
	// Constructs the widget layout
	void Construct(const FArguments& InArgs);

private:
	// The array of asset data displayed in the list view
	TArray<TSharedPtr<FAssetData>> StoredAssetsData;

	// Called by the SListView to generate one row per asset
	TSharedRef<ITableRow> OnGenerateRowForList(
		TSharedPtr<FAssetData> AssetDataToDisplay,
		const TSharedRef<STableViewBase>& OwnerTable
	) const;

	// Builds a checkbox widget per row
	TSharedRef<SCheckBox> ConstructCheckBox(const TSharedPtr<FAssetData>& AssetDataToDisplay) const;

	// Callback for checkbox state change
	void OnCheckBoxStateChanged(ECheckBoxState NewState, TSharedPtr<FAssetData> AssetData) const;

	// Helper function to construct a styled text block for asset class
	TSharedRef<STextBlock> ConstructTextForRowWidget(const FString& TextContent, const FSlateFontInfo FontToUse) const;

	// Helper for consistent font access
	FSlateFontInfo GetEmbossedTextFont() const { return FCoreStyle::Get().GetFontStyle(FName("EmbossedText")); }
};

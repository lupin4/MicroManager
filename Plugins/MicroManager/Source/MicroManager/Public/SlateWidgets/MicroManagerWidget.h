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

	TSharedRef<SListView<TSharedPtr<FAssetData>>> ConstructAssetListView();

	TSharedPtr<SListView<TSharedPtr<FAssetData>>> ConstructedAssetListView;

	// Refresh the list view
	void RefreshAssetListView();


#pragma region RowWidgetForAssetListView
	
	// Called by the SListView to generate one row per asset (non-const)
	TSharedRef<ITableRow> OnGenerateRowForList(
		TSharedPtr<FAssetData> AssetDataToDisplay,
		const TSharedRef<STableViewBase>& OwnerTable
	);

	// Builds a checkbox widget per row
	TSharedRef<SCheckBox> ConstructCheckBox(const TSharedPtr<FAssetData>& AssetDataToDisplay);

	// Callback for checkbox state change
	void OnCheckBoxStateChanged(ECheckBoxState NewState, TSharedPtr<FAssetData> AssetData);

	// Helper function to construct a styled text block for asset class
	TSharedRef<STextBlock> ConstructTextForRowWidget(const FString& TextContent, const FSlateFontInfo FontToUse) const;

	// Button Constructor
	TSharedRef<SButton> ConstructButtonForRowWidget(TSharedPtr<FAssetData>& AssetDataToDisplay);

	// Click Handler for the delete button
	FReply OnDeleteButtonClicked(TSharedPtr<FAssetData> ClickedAssetData);
#pragma endregion


#pragma region Tab Buttons
	
// Constructors for the Buttons
	TSharedRef<SButton> ConstructDeleteAllButton();
	TSharedRef<SButton> ConstructSelectAllButton();
	TSharedRef<SButton> ConstructDeselectAllButton();

	FReply OnDeleteAllButtonClicked();
	FReply OnSelectAllButtonClicked();
	FReply OnDeselectAllButtonClicked();

	TSharedRef<STextBlock> ConstructTextForTabButtons(const FString& TextContent);

#pragma endregion

	// Create an Array to hold assets to be deleted
	TArray<TSharedPtr<FAssetData>> AssetDataToDeleteArray;

	
	// Helper for consistent font access
	FSlateFontInfo GetEmbossedTextFont() const { return FCoreStyle::Get().GetFontStyle(FName("EmbossedText")); }
};



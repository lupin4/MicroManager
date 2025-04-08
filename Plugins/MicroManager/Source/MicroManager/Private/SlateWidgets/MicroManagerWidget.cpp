// Fill out your copyright notice in the Description page of Project Settings.

#include "SlateWidgets/MicroManagerWidget.h"
#include "SlateBasics.h"
#include "DebugHelper.h"

void SMicroManagerTab::Construct(const FArguments& InArgs)
{
	bCanSupportFocus = true;

	// Store the incoming asset data into a member variable
	StoredAssetsData = InArgs._AssetsDataArray;

	DebugHelper::PrintLog(TEXT("MicroManagerTab::Construct called"));
	DebugHelper::PrintLog(FString::Printf(TEXT("StoredAssetsData count: %d"), StoredAssetsData.Num()));

	// If no assets were passed in, insert a dummy asset so the UI doesn't look broken
	if (StoredAssetsData.Num() == 0)
	{
		DebugHelper::ShowNotifyInfo(TEXT("No assets found — adding dummy asset for display"));
		StoredAssetsData.Add(MakeShared<FAssetData>());
	}

	// Set up the font style for the title
	FSlateFontInfo TitleTextFont = FCoreStyle::Get().GetFontStyle(FName("EmbossedText"));
	TitleTextFont.Size = 30;

	// Build the widget layout
	ChildSlot
	[
		// Main vertical Box to hold all the widgets
		SNew(SVerticalBox)

		// First Vertical for Title Text
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(STextBlock)
			.Text(FText::FromString(TEXT("Micro Manager")))
			.Font(TitleTextFont)
			.Justification(ETextJustify::Center)
			.ColorAndOpacity(FSlateColor(FLinearColor::White))
		]

		// Second Vertical Slot for Dropdown Menu (placeholder)
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SHorizontalBox)
		]

		// Third Vertical Slot for Asset List
		+ SVerticalBox::Slot()
		.FillHeight(1.0f) // Make sure list view has space to show
		[
			SNew(SListView<TSharedPtr<FAssetData>>)
			.ItemHeight(24.f)
			.ListItemsSource(&StoredAssetsData)
			.OnGenerateRow(this, &SMicroManagerTab::OnGenerateRowForList)
			.SelectionMode(ESelectionMode::Single)
		]

		// Fourth Vertical Slot for Buttons (placeholder)
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SHorizontalBox)
		]
	];
}

// Callback function for generating each row in the asset list
TSharedRef<ITableRow> SMicroManagerTab::OnGenerateRowForList(
	TSharedPtr<FAssetData> AssetDataToDisplay,
	const TSharedRef<STableViewBase>& OwnerTable) const
{
	// Fallback text if the asset is invalid
	FString DisplayAssetName = TEXT("[Invalid Asset]");

	// If asset is valid, extract its name for display
	if (AssetDataToDisplay.IsValid())
	{
		DisplayAssetName = AssetDataToDisplay->AssetName.ToString();
	}

	DebugHelper::PrintLog(FString::Printf(TEXT("Generating row for: %s"), *DisplayAssetName));

	// Create a horizontal layout for each row: Checkbox + Asset Name
	return SNew(STableRow<TSharedPtr<FAssetData>>, OwnerTable)
	[
		SNew(SHorizontalBox)

		// First slot for checkbox
		+ SHorizontalBox::Slot()
		.HAlign(HAlign_Left)
		.VAlign(VAlign_Center)
		.FillWidth(0.05f)
		[
			ConstructCheckBox(AssetDataToDisplay)
		]

		// Second slot for asset name
		+ SHorizontalBox::Slot()
		[
			SNew(STextBlock)
			.Text(FText::FromString(DisplayAssetName))
		]
	];
}

// Helper function to construct a checkbox widget for each asset
TSharedRef<SCheckBox> SMicroManagerTab::ConstructCheckBox(const TSharedPtr<FAssetData>& AssetDataToDisplay) const
{
	// Create the checkbox and bind the change handler
	return SNew(SCheckBox)
		.Type(ESlateCheckBoxType::CheckBox)
		.OnCheckStateChanged(this, &SMicroManagerTab::OnCheckBoxStateChanged, AssetDataToDisplay)
		.Visibility(EVisibility::Visible);
}

// Handler for when a checkbox is clicked
void SMicroManagerTab::OnCheckBoxStateChanged(ECheckBoxState NewState, TSharedPtr<FAssetData> AssetData) const
{
	// Ensure the asset is valid before reacting
	if (!AssetData.IsValid())
	{
		DebugHelper::PrintLog(TEXT("Checkbox state changed for invalid asset"));
		return;
	}

	// Print the checkbox state to screen
	switch (NewState)
	{
	case ECheckBoxState::Unchecked:
		DebugHelper::Print(AssetData->AssetName.ToString() + TEXT(" is unchecked"), FColor::Red);
		break;

	case ECheckBoxState::Checked:
		DebugHelper::Print(AssetData->AssetName.ToString() + TEXT(" is checked"), FColor::Green);
		break;

	default:
		break;
	}
}

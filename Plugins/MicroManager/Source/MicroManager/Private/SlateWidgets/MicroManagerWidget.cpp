#include "SlateWidgets/MicroManagerWidget.h"
#include "SlateBasics.h"
#include "DebugHelper.h"
#include "MicroManager.h"

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
		SNew(SVerticalBox)

		// Title Text
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(STextBlock)
			.Text(FText::FromString(TEXT("Micro Manager")))
			.Font(TitleTextFont)
			.Justification(ETextJustify::Center)
			.ColorAndOpacity(FSlateColor(FLinearColor::White))
		]

		// Placeholder Dropdown
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SHorizontalBox)
		]

		// Asset List
		+ SVerticalBox::Slot()
		.VAlign(VAlign_Fill)
		[
			SNew(SScrollBox)
			+ SScrollBox::Slot()
			[
				ConstructAssetListView()
			]
		]

		// Placeholder Buttons
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.FillWidth(10.f)
			.Padding(5.f)
			[
				ConstructDeleteAllButton()
				
			]
			+ SHorizontalBox::Slot()
			.FillWidth(10.f)
			.Padding(5.f)
			[
				ConstructSelectAllButton()
				
			]
			+ SHorizontalBox::Slot()
			.FillWidth(10.f)
			.Padding(5.f)
			[
				ConstructDeselectAllButton()
				
			]
		]
	];
}

TSharedRef<SListView<TSharedPtr<FAssetData>>> SMicroManagerTab::ConstructAssetListView()
{
	ConstructedAssetListView = SNew(SListView<TSharedPtr<FAssetData>>)
		.ItemHeight(24.f)
		.ListItemsSource(&StoredAssetsData)
		.OnGenerateRow(this, &SMicroManagerTab::OnGenerateRowForList);
	return ConstructedAssetListView.ToSharedRef();
}

void SMicroManagerTab::RefreshAssetListView()
{
	if (ConstructedAssetListView.IsValid())
	{
		ConstructedAssetListView->RebuildList();
		DebugHelper::PrintLog(TEXT("Asset list refreshed"));
	}
}

#pragma region RowWidgetForAssetListView

TSharedRef<ITableRow> SMicroManagerTab::OnGenerateRowForList(
	TSharedPtr<FAssetData> AssetDataToDisplay,
	const TSharedRef<STableViewBase>& OwnerTable)
{
	const FString DisplayAssetClassName = AssetDataToDisplay->AssetClassPath.GetAssetName().ToString();
	FString DisplayAssetName = TEXT("[Invalid Asset]");

	FSlateFontInfo AssetClassNameFont = GetEmbossedTextFont();
	AssetClassNameFont.Size = 10;

	FSlateFontInfo AssetNameFont = GetEmbossedTextFont();
	AssetNameFont.Size = 15;

	if (AssetDataToDisplay.IsValid())
	{
		DisplayAssetName = AssetDataToDisplay->AssetName.ToString();
	}

	DebugHelper::PrintLog(FString::Printf(TEXT("Generating row for: %s"), *DisplayAssetName));

	return SNew(STableRow<TSharedPtr<FAssetData>>, OwnerTable).Padding(FMargin(2.0f))
	[
		SNew(SHorizontalBox)

		// First Slot for Checkbox
		+ SHorizontalBox::Slot()
		.HAlign(HAlign_Left)
		.VAlign(VAlign_Center)
		.FillWidth(0.05f)
		[
			ConstructCheckBox(AssetDataToDisplay)
		]

		// Second Slot for Asset Class Name
		+ SHorizontalBox::Slot()
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Fill)
		.FillWidth(0.5f)
		[
			ConstructTextForRowWidget(DisplayAssetClassName, AssetClassNameFont)
		]

		// Third slot for Asset Name
		+ SHorizontalBox::Slot()
		.HAlign(HAlign_Left)
		.VAlign(VAlign_Fill)
		[
			ConstructTextForRowWidget(DisplayAssetName, AssetNameFont)
		]

		// Fourth slot for Buttons
		+ SHorizontalBox::Slot()
		.HAlign(HAlign_Right)
		.VAlign(VAlign_Fill)
		[
			ConstructButtonForRowWidget(AssetDataToDisplay)
		]
	];
}

TSharedRef<SCheckBox> SMicroManagerTab::ConstructCheckBox(const TSharedPtr<FAssetData>& AssetDataToDisplay)
{
	return SNew(SCheckBox)
		.Type(ESlateCheckBoxType::CheckBox)
		.OnCheckStateChanged(this, &SMicroManagerTab::OnCheckBoxStateChanged, AssetDataToDisplay)
		.Visibility(EVisibility::Visible);
}

void SMicroManagerTab::OnCheckBoxStateChanged(ECheckBoxState NewState, TSharedPtr<FAssetData> AssetData) 
{
	if (!AssetData.IsValid())
	{
		DebugHelper::PrintLog(TEXT("Checkbox state changed for invalid asset"));
		return;
	}

	switch (NewState)
	{
	case ECheckBoxState::Unchecked:
		AssetDataToDeleteArray.Remove(AssetData);
		
		//DebugHelper::Print(AssetData->AssetName.ToString() + TEXT(" is unchecked"), FColor::Red);
		break;

	case ECheckBoxState::Checked:
		AssetDataToDeleteArray.AddUnique(AssetData);
		//DebugHelper::Print(AssetData->AssetName.ToString() + TEXT(" is checked"), FColor::Green);
		break;

	default:
		break;
	}
}

TSharedRef<STextBlock> SMicroManagerTab::ConstructTextForRowWidget(const FString& TextContent, const FSlateFontInfo FontToUse) const
{
	return SNew(STextBlock)
		.Text(FText::FromString(TextContent))
		.Font(FontToUse)
		.ColorAndOpacity(FSlateColor(FColor::White));
}

// Helper function to construct a delete button for each asset row
TSharedRef<SButton> SMicroManagerTab::ConstructButtonForRowWidget(TSharedPtr<FAssetData>& AssetDataToDisplay)
{
	TSharedRef<SButton> ConstructedButton = SNew(SButton)
		.Text(FText::FromString(TEXT("Delete")))
		.OnClicked(this, &SMicroManagerTab::OnDeleteButtonClicked, AssetDataToDisplay);
	return ConstructedButton;
}

// Callback for the delete button click event
FReply SMicroManagerTab::OnDeleteButtonClicked(TSharedPtr<FAssetData> ClickedAssetData)
{
	FMicroManagerModule& MicroManagerModule = FModuleManager::LoadModuleChecked<FMicroManagerModule>(TEXT("MicroManager"));

	// Call the custom function to delete the clicked asset
	// The TSharedPtr<FAssetData> assetData is passed to the custom function and converted to a raw pointer for deletion
	const bool bAssetDeleted = MicroManagerModule.DeleteSingleAssetForAssetList(*ClickedAssetData.Get());

	// Refresh the asset list to reflect the deletion
	if (bAssetDeleted)
	{
		// Update list source items
		if (StoredAssetsData.Contains(ClickedAssetData))
		{
			StoredAssetsData.Remove(ClickedAssetData);
			DebugHelper::PrintLog(FString::Printf(TEXT("Removed asset: %s"), *ClickedAssetData->AssetName.ToString()));
		}
		
		ConstructedAssetListView->RequestListRefresh();

		DebugHelper::ShowNotifyInfo(FString::Printf(TEXT("Deleted asset: %s"), *ClickedAssetData->AssetName.ToString()));
	}

	return FReply::Handled();
}
#pragma endregion


#pragma region Tab Buttons
TSharedRef<SButton> SMicroManagerTab::ConstructDeleteAllButton()
{
	 TSharedRef<SButton> DeleteAllButton = SNew(SButton)
	 	.ContentPadding(FMargin(5.0f))
	 	.OnClicked(this, &SMicroManagerTab::OnDeleteAllButtonClicked);
	DeleteAllButton->SetContent(ConstructTextForTabButtons(TEXT("Delete All Selected")));
	return DeleteAllButton;
}

TSharedRef<SButton> SMicroManagerTab::ConstructSelectAllButton()
{
	TSharedRef<SButton> SelectAllButton = SNew(SButton)
		 .ContentPadding(FMargin(5.0f))
		 .OnClicked(this, &SMicroManagerTab::OnSelectAllButtonClicked);
	SelectAllButton->SetContent(ConstructTextForTabButtons(TEXT("Select All")));
	return SelectAllButton; 
}

TSharedRef<SButton> SMicroManagerTab::ConstructDeselectAllButton()
{
	TSharedRef<SButton> DeselectAllButton = SNew(SButton)
		 .ContentPadding(FMargin(5.0f))
		 .OnClicked(this, &SMicroManagerTab::OnDeselectAllButtonClicked);
	DeselectAllButton->SetContent(ConstructTextForTabButtons(TEXT("Deselect All")));
	return DeselectAllButton;  
}


FReply SMicroManagerTab::OnDeleteAllButtonClicked()
{
	DebugHelper::Print(TEXT("Deleting all assets..."), FColor::Cyan);
	return FReply::Handled();
}

FReply SMicroManagerTab::OnSelectAllButtonClicked()
{
	DebugHelper::Print(TEXT("Selecting all assets..."), FColor::Cyan);
	return FReply::Handled();
}

FReply SMicroManagerTab::OnDeselectAllButtonClicked()
{
	DebugHelper::Print(TEXT("Deselecting all assets..."), FColor::Cyan);
	return FReply::Handled();
}

TSharedRef<STextBlock> SMicroManagerTab::ConstructTextForTabButtons(const FString& TextContent)
{
	FSlateFontInfo ButtonTextFont = GetEmbossedTextFont();
	TSharedRef<STextBlock> ConstructedTextBlock = SNew(STextBlock)
		.Text(FText::FromString(TextContent))
        .Font(ButtonTextFont)
        .Justification(ETextJustify::Center);
    return ConstructedTextBlock;
}
#pragma endregion
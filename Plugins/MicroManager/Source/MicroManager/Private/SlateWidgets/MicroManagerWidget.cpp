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
		.FillHeight(1.0f)
		[
			SNew(SListView<TSharedPtr<FAssetData>>)
			.ItemHeight(24.f)
			.ListItemsSource(&StoredAssetsData)
			.OnGenerateRow(this, &SMicroManagerTab::OnGenerateRowForList)
			.SelectionMode(ESelectionMode::Single)
		]

		// Placeholder Buttons
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			[
				SNew(STextBlock)
				.Text(FText::FromString(TEXT("Button Placeholder")))
			]
		]
	];
}

TSharedRef<ITableRow> SMicroManagerTab::OnGenerateRowForList(
	TSharedPtr<FAssetData> AssetDataToDisplay,
	const TSharedRef<STableViewBase>& OwnerTable) const
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

	return SNew(STableRow<TSharedPtr<FAssetData>>, OwnerTable)
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
		.FillWidth(0.2f)
		[
			ConstructTextForRowWidget(DisplayAssetClassName, AssetClassNameFont)
		]

		// Third slot for Asset Name
		+ SHorizontalBox::Slot()
		[
			ConstructTextForRowWidget(DisplayAssetName, AssetNameFont)
		]

		//Fourth slot for Buttons
	];
}

TSharedRef<SCheckBox> SMicroManagerTab::ConstructCheckBox(const TSharedPtr<FAssetData>& AssetDataToDisplay) const
{
	return SNew(SCheckBox)
		.Type(ESlateCheckBoxType::CheckBox)
		.OnCheckStateChanged(this, &SMicroManagerTab::OnCheckBoxStateChanged, AssetDataToDisplay)
		.Visibility(EVisibility::Visible);
}

void SMicroManagerTab::OnCheckBoxStateChanged(ECheckBoxState NewState, TSharedPtr<FAssetData> AssetData) const
{
	if (!AssetData.IsValid())
	{
		DebugHelper::PrintLog(TEXT("Checkbox state changed for invalid asset"));
		return;
	}

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

TSharedRef<STextBlock> SMicroManagerTab::ConstructTextForRowWidget(const FString& TextContent, const FSlateFontInfo FontToUse) const
{
	return SNew(STextBlock)
		.Text(FText::FromString(TextContent))
		.Font(FontToUse)
		.ColorAndOpacity(FSlateColor(FColor::White));
}

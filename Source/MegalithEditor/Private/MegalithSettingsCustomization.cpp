#include "MegalithSettingsCustomization.h"
#include "MegalithSettings.h"
#include "MegalithIndexSubsystem.h"
#include "MegalithSourceSubsystem.h"
#include "DetailLayoutBuilder.h"
#include "DetailCategoryBuilder.h"
#include "DetailWidgetRow.h"
#include "Editor.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Text/STextBlock.h"

#define LOCTEXT_NAMESPACE "MegalithSettingsCustomization"

TSharedRef<IDetailCustomization> FMegalithSettingsCustomization::MakeInstance()
{
	return MakeShareable(new FMegalithSettingsCustomization);
}

void FMegalithSettingsCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	IDetailCategoryBuilder& IndexCat = DetailBuilder.EditCategory("Indexing");

	// Re-Index Project button
	IndexCat.AddCustomRow(LOCTEXT("ReindexProjectRow", "Re-Index Project"))
		.NameContent()
		[
			SNew(STextBlock)
			.Text(LOCTEXT("ProjectIndexLabel", "Project Index"))
			.Font(IDetailLayoutBuilder::GetDetailFont())
		]
		.ValueContent()
		[
			SNew(SButton)
			.Text(LOCTEXT("ReindexProjectBtn", "Re-Index Now"))
			.IsEnabled_Lambda([]()
			{
				if (GEditor)
				{
					if (auto* Sub = GEditor->GetEditorSubsystem<UMegalithIndexSubsystem>())
					{
						// Also covers a database that never opened — pressing the
						// button in that state would report success and do nothing.
						return Sub->CanAcceptIndexRequest();
					}
				}
				return false;
			})
			.OnClicked_Lambda([]()
			{
				if (GEditor)
				{
					if (auto* Sub = GEditor->GetEditorSubsystem<UMegalithIndexSubsystem>())
					{
						Sub->StartFullIndex();
					}
				}
				return FReply::Handled();
			})
		];

	// Re-Index Engine Source button
	IndexCat.AddCustomRow(LOCTEXT("ReindexEngineRow", "Re-Index Engine Source"))
		.NameContent()
		[
			SNew(STextBlock)
			.Text(LOCTEXT("EngineSourceLabel", "Engine Source"))
			.Font(IDetailLayoutBuilder::GetDetailFont())
		]
		.ValueContent()
		[
			SNew(SButton)
			.Text(LOCTEXT("ReindexEngineBtn", "Re-Index Now"))
			.IsEnabled_Lambda([]()
			{
				if (GEditor)
				{
					if (auto* Sub = GEditor->GetEditorSubsystem<UMegalithSourceSubsystem>())
					{
						return !Sub->IsIndexing();
					}
				}
				return false;
			})
			.OnClicked_Lambda([]()
			{
				if (GEditor)
				{
					if (auto* Sub = GEditor->GetEditorSubsystem<UMegalithSourceSubsystem>())
					{
						Sub->TriggerReindex();
					}
				}
				return FReply::Handled();
			})
		];
}

#undef LOCTEXT_NAMESPACE

// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "STableRow.h"
#include "IPropertyUtilities.h"
#include "PropertyHandle.h"
#include "LocalizationTargetTypes.h"
#include "LocalizationTargetTypes.h"
#include "SDockTab.h"

class SLocalizationDashboardTargetRow : public SMultiColumnTableRow< TSharedPtr<IPropertyHandle> >
{
public:
	void Construct(const FTableRowArgs& InArgs, const TSharedRef<STableViewBase>& OwnerTableView, const TSharedRef<IPropertyUtilities>& InPropertyUtilities, const TSharedRef<IPropertyHandle>& InTargetObjectPropertyHandle);
	TSharedRef<SWidget> GenerateWidgetForColumn( const FName& ColumnName ) override;

private:
	ULocalizationTarget* GetTarget() const;
	FLocalizationTargetSettings* GetTargetSettings() const;

	FText GetTargetName() const;
	void OnNavigate();

	FText GetCulturesText() const;

	uint32 GetWordCount() const;
	uint32 GetNativeWordCount() const;
	FText GetWordCountText() const;

	void UpdateTargetFromReports();

	bool CanGatherText() const;
	FReply GatherText();

	bool CanImportText() const;
	FReply ImportText();

	bool CanExportText() const;
	FReply ExportText();

	bool CanImportDialogueScript() const;
	FReply ImportDialogueScript();

	bool CanExportDialogueScript() const;
	FReply ExportDialogueScript();

	bool CanImportDialogue() const;
	FReply ImportDialogue();

	bool CanCountWords() const;
	FReply CountWords();

	bool CanCompileText() const;
	FReply CompileText();

	FReply EnqueueDeletion();
	void Delete();

private:
	TSharedPtr<IPropertyUtilities> PropertyUtilities;
	TSharedPtr<IPropertyHandle> TargetObjectPropertyHandle;
	TWeakPtr<SDockTab> TargetEditorDockTab;
};
// Fill out your copyright notice in the Description page of Project Settings.


#include "PromptDialogBox.h"

void UPromptDialogBox::CloseBox_Implementation()
{
	RemoveFromParent();
}

void UPromptDialogBox::BindBtnEvent()
{
	for (auto Btn = BtnMap.CreateIterator(); Btn; ++Btn)
	{
		Btn.Value()->OnClickedDelegate.AddDynamic(this, &UPromptDialogBox::SendDelegate);
	}
}

void UPromptDialogBox::NativeConstruct()
{
	Super::NativeConstruct();

	GenNativeConstruct(this);
	// GenConstruct(this);
	// Execute_GenUpdate(this);
	// GenUpdateGenWidget(this);
}

void UPromptDialogBox::Internal_GenConstruct(UObject* Target)
{
	CreateAndLoadBtnMap();
	BindBtnEvent();
	IConstructGenInterface::Internal_GenConstruct(Target);
}

// /Script/UMGEditor.WidgetBlueprint'/GeneralLibrary/GeneralWidget/PromptDialogBox/WBP_Gen_PromptDialogBoxBtn.WBP_Gen_PromptDialogBoxBtn'

TMap<FString, UPromptDialogBoxBtn*> UPromptDialogBox::CreateAndLoadBtnMap_Implementation()
{
	TMap<FString, UPromptDialogBoxBtn*> LocalBtnMap;
	for (int i = 0; i < SettingInfo.BtnNames.Num(); ++i)
	{
		UPromptDialogBoxBtn* NewBtn = NewObject<UPromptDialogBoxBtn>(GetWorld(), LoadClass<UPromptDialogBoxBtn>(nullptr, *SettingInfo.BtnClassPath));
		NewBtn->CurName = SettingInfo.BtnNames[i];
		NewBtn->BtnSize = SettingInfo.BtnSize[i];
		LocalBtnMap.Add(SettingInfo.BtnNames[i], NewBtn);
	}

	BtnMap = LocalBtnMap;
	return LocalBtnMap;
}

void UPromptDialogBox::SendDelegate_Implementation(UPromptDialogBoxBtn* Btn, const FString& BtnName)
{
	OnClickBtnDelegate.Broadcast(Btn, BtnName);
}

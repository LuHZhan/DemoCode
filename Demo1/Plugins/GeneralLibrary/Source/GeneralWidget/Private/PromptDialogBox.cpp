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

void UPromptDialogBox::GenConstruct(UObject* Target)
{
	CreateAndLoadBtnMap();
	BindBtnEvent();
	IConstructGenInterface::GenConstruct(Target);
}

// /Script/UMGEditor.WidgetBlueprint'/GeneralLibrary/GeneralWidget/PromptDialogBox/WBP_Gen_PromptDialogBoxBtn.WBP_Gen_PromptDialogBoxBtn'

TMap<FString, UPromptDialogBoxBtn*> UPromptDialogBox::CreateAndLoadBtnMap_Implementation()
{
	TMap<FString, UPromptDialogBoxBtn*> LocalBtnMap;
	for (FString BtnName : SettingInfo.BtnNames)
	{
		UPromptDialogBoxBtn* NewBtn = NewObject<UPromptDialogBoxBtn>(GetWorld(), LoadClass<UPromptDialogBoxBtn>(nullptr, *SettingInfo.BtnPath));
		NewBtn->CurName = BtnName;
		NewBtn->BtnSize = SettingInfo.BtnSize;
		LocalBtnMap.Add(BtnName, NewBtn);
	}

	BtnMap = LocalBtnMap;
	return LocalBtnMap;
}

void UPromptDialogBox::SendDelegate_Implementation(UPromptDialogBoxBtn* Btn, const FString& BtnName)
{
	OnClickBtnDelegate.Broadcast(Btn, BtnName);
}

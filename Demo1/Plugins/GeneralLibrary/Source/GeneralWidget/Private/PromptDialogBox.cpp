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
	CreateAndLoadBtnMap();
	BindBtnEvent();
	
	Execute_GenUpdate(this);
	UpdateGenWidget(this);
}

TMap<FString, UPromptDialogBoxBtn*> UPromptDialogBox::CreateAndLoadBtnMap_Implementation()
{
	TMap<FString, UPromptDialogBoxBtn*> LocalBtnMap;

	for (FString BtnName : SettingInfo.BtnNames)
	{
		UPromptDialogBoxBtn* NewBtn = CreateWidget<UPromptDialogBoxBtn>(GetWorld());
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

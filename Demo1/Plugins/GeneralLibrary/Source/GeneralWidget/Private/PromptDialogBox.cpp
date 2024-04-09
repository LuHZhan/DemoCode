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
	BindBtnEvent();
}

void UPromptDialogBox::SendDelegate_Implementation(UPromptDialogBoxBtn* Btn, const FString& BtnName)
{
	OnClickBtnDelegate.Broadcast(Btn, BtnName);
}

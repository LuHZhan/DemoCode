// Fill out your copyright notice in the Description page of Project Settings.


#include "PromptDialogBoxBtn.h"

void UPromptDialogBoxBtn::BtnBind()
{
	Btn->OnClicked.AddDynamic(this, &UPromptDialogBoxBtn::OnClicked);
}

void UPromptDialogBoxBtn::NativeConstruct()
{
	Super::NativeConstruct();
	BtnBind();
}

void UPromptDialogBoxBtn::OnClicked_Implementation()
{
	OnClickedDelegate.Broadcast(this, BtnName);
}

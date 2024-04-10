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

	Execute_GenUpdate(this);
	UpdateGenWidget(this);
}

void UPromptDialogBoxBtn::OnClicked_Implementation()
{
	OnClickedDelegate.Broadcast(this, CurName);
}

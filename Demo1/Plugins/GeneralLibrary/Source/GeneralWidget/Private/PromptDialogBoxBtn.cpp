// Fill out your copyright notice in the Description page of Project Settings.


#include "PromptDialogBoxBtn.h"

#include "Kismet/KismetSystemLibrary.h"

void UPromptDialogBoxBtn::BtnBind()
{
	Btn->OnClicked.AddDynamic(this, &UPromptDialogBoxBtn::OnClicked);
}

void UPromptDialogBoxBtn::NativeConstruct()
{
	Super::NativeConstruct();

	GenNativeConstruct(this);
	// GenConstruct(this);
	// Execute_GenUpdate(this);
	// GenUpdateGenWidget(this);
}

void UPromptDialogBoxBtn::GenConstruct(UObject* Target)
{
	BtnBind();
	IConstructGenInterface::GenConstruct(this);
}

void UPromptDialogBoxBtn::OnClicked_Implementation()
{
	OnClickedDelegate.Broadcast(this, CurName);
}

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

	// GenNativeConstruct(this);
	Internal_GenConstruct(this);
	// Execute_GenUpdate(this);
	// GenUpdateGenWidget(this);
}

void UPromptDialogBoxBtn::Internal_GenConstruct(UObject* Target)
{
	BtnBind();
	IConstructGenInterface::Internal_GenConstruct(Target);
}

void UPromptDialogBoxBtn::OnClicked_Implementation()
{
	OnClickedDelegate.Broadcast(this, CurName);
}

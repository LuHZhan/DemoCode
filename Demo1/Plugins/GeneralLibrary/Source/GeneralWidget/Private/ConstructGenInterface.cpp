// Fill out your copyright notice in the Description page of Project Settings.


#include "ConstructGenInterface.h"
#include "Blueprint/WidgetTree.h"

// Add default functionality here for any IConstructGenInterface functions that are not pure virtual.

void IConstructGenInterface::GenUpdateGenWidget(UUserWidget* CurUserWidget)
{
	Execute_GenUpdate(CurUserWidget);
	TArray<UWidget*> GenWidgetList;
	GetAllGenWidget(CurUserWidget, GenWidgetList);
	
	if (GenWidgetList.IsEmpty())
	{
		return;
	}
	for (UWidget* Widget : GenWidgetList)
	{
		UUserWidget* UserWidget = Cast<UUserWidget>(Widget);
		GenUpdateGenWidget(UserWidget);
	}
}

void IConstructGenInterface::GetAllGenWidget(UUserWidget* CurUserWidget, TArray<UWidget*>& WidgetList)
{
	if (CurUserWidget)
	{
		TArray<UWidget*> CurWidgetList;
		CurUserWidget->WidgetTree.Get()->GetAllWidgets(CurWidgetList);
		for (UWidget* Widget : CurWidgetList)
		{
			if (Widget->GetClass()->ImplementsInterface(UConstructGenInterface::StaticClass()))
			{
				WidgetList.Add(Widget);
			}
		}
	}
}

void IConstructGenInterface::Internal_GenConstruct(UObject* Target)
{
	if (Target->GetClass()->ImplementsInterface(UConstructGenInterface::StaticClass()))
	{
		Execute_GenConstruct(Target);
	}
}

void IConstructGenInterface::GenNativeConstruct(UUserWidget* Target)
{
	Internal_GenConstruct(Target);
	GenUpdateGenWidget(Target);
}

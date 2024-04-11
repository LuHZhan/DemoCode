// Fill out your copyright notice in the Description page of Project Settings.


#include "ConstructGenInterface.h"
#include "Blueprint/WidgetTree.h"

// Add default functionality here for any IConstructGenInterface functions that are not pure virtual.

void IConstructGenInterface::GenUpdateGenWidget(UUserWidget* CurUserWidget)
{
	TArray<UWidget*> GenWidgetList;
	GetAllGenWidget(CurUserWidget, GenWidgetList);
	for (auto Widget : GenWidgetList)
	{
		Execute_GenUpdate(Widget);
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

void IConstructGenInterface::GenConstruct(UObject* Target)
{
	if (Target->GetClass()->ImplementsInterface(UConstructGenInterface::StaticClass()))
	{
		Execute_BPGenConstruct(Target);
	}
}

void IConstructGenInterface::GenNativeConstruct(UUserWidget* Target)
{
	GenConstruct(Target);
	Execute_GenUpdate(Target);
	GenUpdateGenWidget(Target);
}

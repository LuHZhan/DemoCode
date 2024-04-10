// Fill out your copyright notice in the Description page of Project Settings.


#include "ConstructGenInterface.h"
#include "Blueprint/WidgetTree.h"

// Add default functionality here for any IConstructGenInterface functions that are not pure virtual.

void IConstructGenInterface::UpdateGenWidget(UUserWidget* CurUserWidget)
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

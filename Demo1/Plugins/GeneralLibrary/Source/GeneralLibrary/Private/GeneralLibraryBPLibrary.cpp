// Copyright Epic Games, Inc. All Rights Reserved.

#include "GeneralLibraryBPLibrary.h"

#include "ConstructGenInterface.h"
#include "GeneralLibrary.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetTree.h"

UGeneralLibraryBPLibrary::UGeneralLibraryBPLibrary(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

float UGeneralLibraryBPLibrary::GeneralLibrarySampleFunction(float Param)
{
	return -1;
}

bool UGeneralLibraryBPLibrary::SetTextBlockInfo(UTextBlock* Widget, FTextFontInfo TextFontInfo)
{
	if (Widget)
	{
		FSlateFontInfo NewInfo = Widget->GetFont();
		NewInfo.Size = TextFontInfo.Size;
		Widget->SetFont(NewInfo);
		Widget->SetText(TextFontInfo.Text);
		Widget->SetColorAndOpacity(TextFontInfo.Color);
		return true;
	}
	return false;
}

void UGeneralLibraryBPLibrary::UpdateGenWidget(UUserWidget* CurUserWidget)
{
	if (CurUserWidget->GetClass()->ImplementsInterface(UConstructGenInterface::StaticClass()))
	{
		IConstructGenInterface::Execute_GenUpdate(CurUserWidget);
	}

	TArray<UWidget*> Widgets;
	CurUserWidget->WidgetTree.Get()->GetAllWidgets(Widgets);
	for (auto Widget : Widgets)
	{
		if (Widget->GetClass()->ImplementsInterface(UConstructGenInterface::StaticClass()))
		{
			IConstructGenInterface::Execute_GenUpdate(Widget);
		}
	}
}

TSubclassOf<UObject> UGeneralLibraryBPLibrary::PathIsValid(const FString& Path)
{
	return LoadClass<UObject>(nullptr, *Path);
}

FVector2D UGeneralLibraryBPLibrary::GetWidgetLocalPosition(UWidget* PanelWidget, UWidget* TargetWidget)
{
	if (PanelWidget)
	{
		const FGeometry Geometry = PanelWidget->GetCachedGeometry();
		FVector2D Size = TargetWidget->GetCachedGeometry().GetLocalSize() / 2.0f;
		const FVector2D Position = Geometry.AbsoluteToLocal(TargetWidget->GetCachedGeometry().GetAbsolutePosition()) + Size;

		return Position;
	}
	return FVector2D::ZeroVector;
}

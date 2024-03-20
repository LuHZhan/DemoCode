// Fill out your copyright notice in the Description page of Project Settings.


#include "TraceUIViewport.h"

void UTraceUIViewport::Toggle(bool bIsOpen)
{
	bIsStartAnchoring = bIsOpen;
	for (const TTuple<FString, FPlaneMoveData> Element : UIData)
	{
		if (Element.Value.UI->IsVisible() != bIsOpen)
		{
			Element.Value.UI->SetVisibility(bIsOpen ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
		}
	}
}

void UTraceUIViewport::AddOrUpdateUI(const FString& Name, TWeakObjectPtr<UUserWidget> UI, const FVector2D& XY)
{
	if (UIData.Contains(Name))
	{
		UIData[Name].UI = UI;
		UIData[Name].XY = XY;
	}
	else
	{
		UIData.Add(Name, FPlaneMoveData(UI, XY));
		UpdateCanvas();
	}
	// UpdateCanvas();
}

void UTraceUIViewport::RemoveUI(const FString& Name)
{
	UIData.Remove(Name);
	UpdateCanvas();
}

void UTraceUIViewport::ClearAllUI()
{
	UIData.Empty();
	UpdateCanvas();
}

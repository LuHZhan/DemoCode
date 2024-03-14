// Fill out your copyright notice in the Description page of Project Settings.


#include "TracUIViewport.h"

void UTracUIViewport::AddOrUpdateUI(const FString& Name, TWeakObjectPtr<UUserWidget> UI, const FVector2D& XY)
{
	if (UIData.Contains(Name))
	{
		UIData[Name].UI = UI;
		UIData[Name].XY = XY;
	}
	else
	{
		UIData.Add(Name, FPlaneMoveData(UI, XY));
	}
	UpdateCanvas();
}

void UTracUIViewport::RemoveUI(const FString& Name)
{
	UIData.Remove(Name);
	UpdateCanvas();
}

void UTracUIViewport::ClearAllUI()
{
	UIData.Empty();
	UpdateCanvas();
}

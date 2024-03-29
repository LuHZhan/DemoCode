// Fill out your copyright notice in the Description page of Project Settings.


#include "TraceUIViewport.h"

void UTraceUIViewport::Toggle(bool bIsOpen)
{
	bIsStartAnchoring = bIsOpen;
	for (const TTuple<FString, FPlaneMoveData> Element : UIData)
	{
		if (Element.Value.NormalUI->IsVisible() != bIsOpen)
		{
			Element.Value.NormalUI->SetVisibility(bIsOpen ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
		}
	}
}

void UTraceUIViewport::AddOrUpdateUI(const FString& Name, TWeakObjectPtr<UUserWidget> NormalUI, TWeakObjectPtr<UUserWidget> LimitUI,
                                     EUIStyleType CurStyleType, const FVector4& Data)
{
	if (UIData.Contains(Name))
	{
		// TODO: 在Update的时候,数据也需要更新
		UIData[Name].NormalUI = NormalUI;
		UIData[Name].Data = Data;
	}
	else
	{
		UIData.Add(Name, FPlaneMoveData(NormalUI, LimitUI, Data, CurStyleType));
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

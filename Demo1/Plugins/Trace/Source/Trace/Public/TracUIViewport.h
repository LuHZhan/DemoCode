// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

// #include "TraceDefinition.h"

#include "TracUIViewport.generated.h"

/**
 * 
 */

USTRUCT(Blueprintable, BlueprintType)
struct FPlaneMoveData
{
	GENERATED_BODY()

	FPlaneMoveData(): UI(nullptr), XY(0.0)
	{
	};

	FPlaneMoveData(const TWeakObjectPtr<UUserWidget>& UI, const FVector2D& XY) : UI(UI), XY(XY)
	{
	};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TWeakObjectPtr<UUserWidget> UI;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D XY;
};

UCLASS(Blueprintable, BlueprintType)
class TRACE_API UTracUIViewport : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(Blueprintable, BlueprintImplementableEvent, Category="TraceUIViewport")
	void Anchoring();

	UFUNCTION(Blueprintable, BlueprintImplementableEvent, Category="TraceUIViewport")
	void UpdateCanvas();

	UFUNCTION(Blueprintable, Category="TraceUIViewport")
	void AddOrUpdateUI(const FString& Name, TWeakObjectPtr<UUserWidget> UI, const FVector2D& XY);

	UFUNCTION(Blueprintable, Category="TraceUIViewport")
	void RemoveUI(const FString& Name);

	UFUNCTION(Blueprintable, Category="TraceUIViewport")
	void ClearAllUI();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="TraceUIViewport")
	TMap<FString, FPlaneMoveData> UIData;
};

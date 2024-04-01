// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "TraceDefinition.h"

#include "TraceUIViewport.generated.h"

/**
 * 
 */

USTRUCT(Blueprintable, BlueprintType)
struct FPlaneMoveData
{
	GENERATED_BODY()

	FPlaneMoveData(): NormalUI(nullptr), LimitUI(nullptr), SelectedUI(nullptr), CurStyleType(EUIStyleType::Normal), Data(0.0)
	{
	};

	FPlaneMoveData(const TWeakObjectPtr<UUserWidget>& NewNormalUI, const TWeakObjectPtr<UUserWidget>& NewLimitUI, const FVector4& NewData,
	               EUIStyleType NewStyle) : NormalUI(NewNormalUI),
	                                        LimitUI(NewLimitUI),
	                                        CurStyleType(NewStyle),
	                                        Data(NewData)
	{
		FlushSelectedUI();
	};
	
	void FlushSelectedUI();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TWeakObjectPtr<UUserWidget> NormalUI;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TWeakObjectPtr<UUserWidget> LimitUI;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TWeakObjectPtr<UUserWidget> SelectedUI;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EUIStyleType CurStyleType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector4 Data;
};

// TODO: 这里关于NormalUI和LimitUI的实现需要抽象出来，目前都耦合在一起了，拓展只能通过重写函数的方式实现
UCLASS(Blueprintable, BlueprintType)
class TRACE_API UTraceUIViewport : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="TraceUIViewport")
	void Toggle(bool bIsOpen);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category="TraceUIViewport")
	void Moving();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category="TraceUIViewport")
	void LoadCanvas();

	UFUNCTION(Category="TraceUIViewport")
	void AddOrUpdateUI(const FString& Name, TWeakObjectPtr<UUserWidget> NormalUI, TWeakObjectPtr<UUserWidget> LimitUI, EUIStyleType CurStyleType,
	                   const FVector4& Data);

	UFUNCTION(BlueprintCallable, Category="TraceUIViewport")
	void RemoveUI(const FString& Name);

	UFUNCTION(BlueprintCallable, Category="TraceUIViewport")
	void ClearAllUI();

	UFUNCTION(BlueprintCallable)
	void FlushAllSelectedUI();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="TraceUIViewport")
	TMap<FString, FPlaneMoveData> UIData;

	UPROPERTY(BlueprintReadOnly, Category="TraceUIViewport")
	bool bIsStartAnchoring = false;
};

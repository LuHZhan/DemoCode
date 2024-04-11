// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Blueprint/UserWidget.h"
#include "ConstructGenInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, Blueprintable, BlueprintType)
class UConstructGenInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class GENERALWIDGET_API IConstructGenInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	/**
	 * 用于处理组合导致多次构造问题
	 */
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void GenUpdate();

	/**
	 * 调用CurUserWidget下所有接口成员的GenUpdate()
	 * @param CurUserWidget 当前控件
	 */
	virtual void GenUpdateGenWidget(UUserWidget* CurUserWidget);
	/**
	 * 获取CurUserWidget下所有接口成员，返回到WidgetList
	 * @param CurUserWidget 当前控件
	 * @param WidgetList 返回容器
	 */
	virtual void GetAllGenWidget(UUserWidget* CurUserWidget, TArray<UWidget*>& WidgetList);

	/**
	 * 用于处理C++父类无法重写Construct的构造顺序问题
	 */
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void BPGenConstruct();
	/**
	 * 默认调用BPGenInit
	 * @param Target 当前对象
	 */
	virtual void GenConstruct(UObject* Target);

	/**
	 * 调用 GenConstruct(Target)--Execute_GenUpdate(Target)--GenUpdateGenWidget(Target)
	 * @param Target 当前对象
	 */
	virtual void GenNativeConstruct(UUserWidget* Target);
};

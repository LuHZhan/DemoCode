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
 * TODO: 在PreConstruct调用接口会导致保存到50%卡死
 */
class GENERALWIDGET_API IConstructGenInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	/**
	 * 用于处理组合导致构造先后问题，所有通用组件依赖数据的可视性初始化都应该写在这里
	 * （会被多次调用）
	 */
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void GenUpdate();

	/**
	 * 初始化自己，并递归的初始化所有的成员通用组件
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
	 * 用于处理C++父类无法重写Construct的构造顺序问题，所有通用组件依赖父子初始化顺序的初始化都应该写在这里
	 * （只会被调用一次）
	 */
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void GenConstruct();
	/**
	 * 默认调用BPGenInit
	 * @param Target 当前对象
	 */
	virtual void Internal_GenConstruct(UObject* Target);

	/**
	 * 调用 GenConstruct(Target)--Execute_GenUpdate(Target)--GenUpdateGenWidget(Target)
	 * @param Target 当前对象
	 */
	virtual void GenNativeConstruct(UUserWidget* Target);
};

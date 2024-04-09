// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "WidgetDefinition.h"
#include "PromptDialogBoxBtn.h"

#include "PromptDialogBox.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnClickBtnDelegate, UPromptDialogBoxBtn*, Btn, const FString&, BtnName);

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class GENERALWIDGET_API UPromptDialogBox : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void CloseBox();

	void BindBtnEvent();

	UFUNCTION(BlueprintNativeEvent)
	void SendDelegate(UPromptDialogBoxBtn* Btn, const FString& BtnName);

	virtual void NativeConstruct() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ExposeOnSpawn=true))
	FPromptDialogBoxSettingInfo SettingInfo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FString, UPromptDialogBoxBtn*> BtnMap;

	UPROPERTY(BlueprintAssignable)
	FOnClickBtnDelegate OnClickBtnDelegate;
};

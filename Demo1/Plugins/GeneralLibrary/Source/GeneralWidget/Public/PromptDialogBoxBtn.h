// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "PromptDialogBoxBtn.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnClickedDelegate, UPromptDialogBoxBtn*, Btn, const FString&, BtnName);

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class GENERALWIDGET_API UPromptDialogBoxBtn : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void BtnBind();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnClicked();

	virtual void NativeConstruct() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ExposeOnSpawn=true))
	FString BtnName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UButton* Btn;

	UPROPERTY(BlueprintAssignable)
	FOnClickedDelegate OnClickedDelegate;
};

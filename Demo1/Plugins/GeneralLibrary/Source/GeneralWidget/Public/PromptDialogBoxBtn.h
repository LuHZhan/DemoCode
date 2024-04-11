// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"

#include "ConstructGenInterface.h"
#include "WidgetDefinition.h"

#include "PromptDialogBoxBtn.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnClickedDelegate, UPromptDialogBoxBtn*, Btn, const FString&, BtnName);

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class GENERALWIDGET_API UPromptDialogBoxBtn : public UUserWidget, public IConstructGenInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void BtnBind();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnClicked();

	virtual void NativeConstruct() override;

	
	virtual void GenConstruct(UObject* Target) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ExposeOnSpawn=true), Category="Config")
	FString CurName = "PromptDialogBox";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ExposeOnSpawn=true), Category="Config")
	FVector2D BtnSize = {400, 200};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ExposeOnSpawn=true), Category="Config")
	FTextFontInfo TextInfo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config")
	UButton* Btn;

	UPROPERTY(BlueprintAssignable)
	FOnClickedDelegate OnClickedDelegate;
};

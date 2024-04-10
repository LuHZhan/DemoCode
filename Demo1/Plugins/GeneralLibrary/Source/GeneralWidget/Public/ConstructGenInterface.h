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
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void GenUpdate();
	
	virtual void UpdateGenWidget(UUserWidget* CurUserWidget);
	virtual void GetAllGenWidget(UUserWidget* CurUserWidget, TArray<UWidget*>& WidgetList);
};

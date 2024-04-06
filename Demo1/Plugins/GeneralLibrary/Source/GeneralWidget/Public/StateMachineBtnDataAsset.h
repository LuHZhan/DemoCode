// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"

#include "WidgetDefinition.h"

#include "StateMachineBtnDataAsset.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class GENERALWIDGET_API UStateMachineBtnDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category=StateMachineBtn)
	FStateMachineInfo StateMachineInfo;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"

#include "ItemsDefinition.h"

#include "QualityTextureDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class ITEMS_API UQualityTextureDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, Category=StateMachineBtn)
	FQualityTextureInfo QualityTextureInfo;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "BackpackDefinition.h"
#include "ItemsDefinition.h"

#include "BackpackFunLibrary.generated.h"
/**
 * 
 */
UCLASS()
class BACKPACK_API UBackpackFunLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Backpack")
	static TArray<FItemsArray> FiltrationFromTotalItemsInfo(const TArray<FItemsInfo>& TotalItemsInfo, const FBackpackSettingInfo& SettingInfo);
	

};

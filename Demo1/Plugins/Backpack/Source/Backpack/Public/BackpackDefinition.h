// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "MathDefinition.h"

#include "BackpackDefinition.generated.h"

UCLASS()
class BACKPACK_API ABackpackDefinition : public AActor
{
	GENERATED_BODY()
};

USTRUCT(Blueprintable, BlueprintType)
struct FBackpackSettingInfo
{
	GENERATED_BODY()

	/**
	 * X:最大列数
	 * Y:最大行数
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FIntVector2D MaxSize = {8, 99};
};

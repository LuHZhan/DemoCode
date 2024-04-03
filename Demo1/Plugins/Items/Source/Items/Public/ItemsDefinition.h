// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemsDefinition.generated.h"

UCLASS()
class ITEMS_API AItemsDefinition : public AActor
{
	GENERATED_BODY()
};

UENUM(Blueprintable, BlueprintType)
enum class EQuality:uint8
{
	//普通 -- 炮灰
	Normal,
	//少见 -- 路人
	Uncommon,
	//优秀 -- 工具人
	Superior ,
	//史诗 -- 配角
	Epic,
	//传奇 -- 反派
	Legendary,
	//唯一 -- 主角
	Unique,
};

USTRUCT(Blueprintable, BlueprintType)
struct FItemsInfo
{
	GENERATED_BODY()


	UPROPERTY(EditAnywhere, Blueprintable)
	EQuality Quality;
};

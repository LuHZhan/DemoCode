// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

// #include "GeneralDefinition.h"
#include "WidgetDefinition.h"

#include "ItemsDefinition.generated.h"

UCLASS()
class ITEMS_API AItemsDefinition : public AActor
{
	GENERATED_BODY()
};

UENUM(Blueprintable, BlueprintType)
enum class EQuality:uint8
{
	None,
	//普通 -- 炮灰
	Normal,
	//少见 -- 路人
	Uncommon,
	//优秀 -- 工具人
	Superior,
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString ID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString FunctionName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString VisibleName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Number;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int MaxNumber;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EQuality Quality;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* Icon;
};

USTRUCT(Blueprintable, BlueprintType)
struct FStateTextureInfo
{
	GENERATED_BODY()

	FStateTextureInfo()
	{
		StateTextureInfo.Empty();
		StateTextureInfo.Add(EBtnStateType::Start, nullptr);
		StateTextureInfo.Add(EBtnStateType::Normal, nullptr);
		StateTextureInfo.Add(EBtnStateType::Pressed, nullptr);
		StateTextureInfo.Add(EBtnStateType::Released, nullptr);
		StateTextureInfo.Add(EBtnStateType::Clicked, nullptr);
		StateTextureInfo.Add(EBtnStateType::Hovered, nullptr);
		StateTextureInfo.Add(EBtnStateType::Unhovered, nullptr);
		StateTextureInfo.Add(EBtnStateType::Close, nullptr);
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<EBtnStateType, UTexture2D*> StateTextureInfo;
};

USTRUCT(Blueprintable, BlueprintType)
struct FQualityTextureInfo
{
	GENERATED_BODY()

	FQualityTextureInfo()
	{
		QualityTextureInfo.Empty();
		QualityTextureInfo.Add(EQuality::Normal, FStateTextureInfo());
		QualityTextureInfo.Add(EQuality::Uncommon, FStateTextureInfo());
		QualityTextureInfo.Add(EQuality::Superior, FStateTextureInfo());
		QualityTextureInfo.Add(EQuality::Epic, FStateTextureInfo());
		QualityTextureInfo.Add(EQuality::Legendary, FStateTextureInfo());
		QualityTextureInfo.Add(EQuality::Unique, FStateTextureInfo());
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<EQuality, FStateTextureInfo> QualityTextureInfo;
};

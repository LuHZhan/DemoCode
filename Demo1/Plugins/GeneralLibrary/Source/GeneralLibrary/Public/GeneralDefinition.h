// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GeneralDefinition.generated.h"

UCLASS()
class GENERALLIBRARY_API AGeneralDefinition : public AActor
{
	GENERATED_BODY()
};

UENUM(Blueprintable, BlueprintType)
enum class EStateType:uint8
{
	Normal,
	Pressed,
	Released,
	Hovered,
	Unhovered,
	DoubleClicked,
};

UENUM(Blueprintable, BlueprintType)
enum class EClickedType:uint8
{
	Highlight,
	Switch,
	Close,
};

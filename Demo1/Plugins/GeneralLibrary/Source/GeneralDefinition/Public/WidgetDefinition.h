// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WidgetDefinition.generated.h"

UCLASS()
class GENERALDEFINITION_API AWidgetDefinition : public AActor
{
	GENERATED_BODY()
};

UENUM(Blueprintable, BlueprintType)
enum class EBtnStateType:uint8
{
	None,
	Start,
	Normal,
	Pressed,
	Released,
	Hovered,
	Unhovered,
	Close,
};

UENUM(Blueprintable, BlueprintType)
enum class EBtnActionType:uint8
{
	NoneAction,
	PressedAction,
	ReleasedAction,
	HoveredAction,
	UnhoveredAction,
};

UENUM(Blueprintable, BlueprintType)
enum class EBtnClickedType:uint8
{
	None,
	Highlight,
	NoHoverHighlight,
	Switch,
	NoHoverSwitch,
	Freedom,
	Close,
};


USTRUCT(Blueprintable, BlueprintType)
struct FStateAfterActionPerFormed
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<EBtnActionType, EBtnStateType> ActionWithState;
};

USTRUCT(Blueprintable, BlueprintType)
struct FStateInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<EBtnStateType, FStateAfterActionPerFormed> ActionWithStatePair;
};

USTRUCT(Blueprintable, BlueprintType)
struct FStateMachineInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<EBtnClickedType, FStateInfo> Info;
};

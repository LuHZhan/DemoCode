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
	// TODO: 这里加入Clicked是因为使用Pressed和Released会导致Switch的模式不好实现，两种点击都走的Pressed的通道
	Clicked,
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
	// TODO: 这里加入Clicked是因为使用Pressed和Released会导致Switch的模式不好实现，两种点击都走的Pressed的通道
	ClickedAction,
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

	FStateAfterActionPerFormed()
	{
		ActionWithState.Empty();
		ActionWithState.Add(EBtnActionType::PressedAction, EBtnStateType::None);
		ActionWithState.Add(EBtnActionType::ReleasedAction, EBtnStateType::None);
		ActionWithState.Add(EBtnActionType::ClickedAction, EBtnStateType::None);
		ActionWithState.Add(EBtnActionType::HoveredAction, EBtnStateType::None);
		ActionWithState.Add(EBtnActionType::UnhoveredAction, EBtnStateType::None);
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<EBtnActionType, EBtnStateType> ActionWithState;
};

USTRUCT(Blueprintable, BlueprintType)
struct FStateInfo
{
	GENERATED_BODY()

	FStateInfo()
	{
		ActionWithStatePair.Empty();
		ActionWithStatePair.Add(EBtnStateType::Start, FStateAfterActionPerFormed());
		ActionWithStatePair.Add(EBtnStateType::Normal, FStateAfterActionPerFormed());
		ActionWithStatePair.Add(EBtnStateType::Pressed, FStateAfterActionPerFormed());
		ActionWithStatePair.Add(EBtnStateType::Released, FStateAfterActionPerFormed());
		ActionWithStatePair.Add(EBtnStateType::Clicked, FStateAfterActionPerFormed());
		ActionWithStatePair.Add(EBtnStateType::Hovered, FStateAfterActionPerFormed());
		ActionWithStatePair.Add(EBtnStateType::Unhovered, FStateAfterActionPerFormed());
		ActionWithStatePair.Add(EBtnStateType::Close, FStateAfterActionPerFormed());
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<EBtnStateType, FStateAfterActionPerFormed> ActionWithStatePair;
};

USTRUCT(Blueprintable, BlueprintType)
struct FStateMachineInfo
{
	GENERATED_BODY()

	FStateMachineInfo()
	{
		Info.Empty();
		Info.Add(EBtnClickedType::Highlight, FStateInfo());
		Info.Add(EBtnClickedType::NoHoverHighlight, FStateInfo());
		Info.Add(EBtnClickedType::Switch, FStateInfo());
		Info.Add(EBtnClickedType::NoHoverSwitch, FStateInfo());
		Info.Add(EBtnClickedType::Freedom, FStateInfo());
		Info.Add(EBtnClickedType::Close, FStateInfo());
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<EBtnClickedType, FStateInfo> Info;
};

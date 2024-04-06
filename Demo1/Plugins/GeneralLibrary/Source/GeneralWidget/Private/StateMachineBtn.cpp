// Fill out your copyright notice in the Description page of Project Settings.


#include "StateMachineBtn.h"

void UStateMachineBtn::NativeConstruct()
{
	Super::NativeConstruct();

	// if (bIsInit)
	// {
	// 	Init();
	// }
}

void UStateMachineBtn::Init()
{
	ActionBtn->OnPressed.AddDynamic(this, &UStateMachineBtn::OnPressed);
	ActionBtn->OnReleased.AddDynamic(this, &UStateMachineBtn::OnReleased);
	ActionBtn->OnHovered.AddDynamic(this, &UStateMachineBtn::OnHovered);
	ActionBtn->OnUnhovered.AddDynamic(this, &UStateMachineBtn::OnUnhovered);

	// StateIndexMap.Empty();
	// StateIndexMap.Add(EBtnStateType::Start, 0);
	// StateIndexMap.Add(EBtnStateType::Hovered, 1);
	// StateIndexMap.Add(EBtnStateType::Pressed, 2);
	// StateIndexMap.Add(EBtnStateType::Released, 3);
	// StateIndexMap.Add(EBtnStateType::Unhovered, 4);
	// StateIndexMap.Add(EBtnStateType::Unhovered, 5);
}

FStateMachineInfo UStateMachineBtn::GetDataAsset() const
{
	return DA_StateMachine->StateMachineInfo;
}

EBtnStateType UStateMachineBtn::GetStateFromDataAsset(EBtnActionType NewAction, bool& bIsContainer) const
{
	EBtnStateType Result = EBtnStateType::None;
	bIsContainer = false;

	if (DA_StateMachine)
	{
		if (const FStateInfo* InfoList = DA_StateMachine->StateMachineInfo.Info.Find(CurClickedType); InfoList)
		{
			if (const FStateAfterActionPerFormed* ActionMap = InfoList->ActionWithStatePair.Find(CurStateType); ActionMap)
			{
				if (const EBtnStateType* NewState = ActionMap->ActionWithState.Find(NewAction); ActionMap)
				{
					Result = *NewState;
					bIsContainer = true;
				}
			}
		}
	}
	return Result;
}

void UStateMachineBtn::OnPressed()
{
	SetAction(EBtnActionType::PressedAction);
	TransitionState(CurActionType);
}

void UStateMachineBtn::OnReleased()
{
	SetAction(EBtnActionType::ReleasedAction);
	TransitionState(CurActionType);
}

void UStateMachineBtn::OnHovered()
{
	SetAction(EBtnActionType::HoveredAction);
	TransitionState(CurActionType);
}

void UStateMachineBtn::OnUnhovered()
{
	SetAction(EBtnActionType::UnhoveredAction);
	TransitionState(CurActionType);
}

void UStateMachineBtn::SetState(EBtnStateType NewState)
{
	CurStateType = NewState;
}

void UStateMachineBtn::SetAction(EBtnActionType NewAction)
{
	CurActionType = NewAction;
}

bool UStateMachineBtn::CheckIsTransitionAction_Implementation()
{
	bool bIsValid = true;
	if ((CurClickedType == EBtnClickedType::NoHoverHighlight || CurClickedType == EBtnClickedType::NoHoverSwitch)
		&& (CurStateType == EBtnStateType::Hovered || CurStateType == EBtnStateType::Unhovered))
	{
		bIsValid = false;
	}
	else if ((CurClickedType == EBtnClickedType::Highlight || CurClickedType == EBtnClickedType::NoHoverHighlight) &&
		CurStateType == EBtnStateType::Released && CurActionType == EBtnActionType::ReleasedAction)
	{
		bIsValid = false;
	}
	else if (CurClickedType == EBtnClickedType::Close)
	{
		bIsValid = false;
	}

	return bIsValid;
}

EBtnStateType UStateMachineBtn::TransitionState_Implementation(EBtnActionType NewAction)
{
	// SetState(NewState);
	// ForceUpdateStateFromClickedType();
	return CurStateType;
}

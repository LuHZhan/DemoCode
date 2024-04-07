// Fill out your copyright notice in the Description page of Project Settings.


#include "StateMachineBtn.h"

void UStateMachineBtn::NativeConstruct()
{
	Super::NativeConstruct();
}

void UStateMachineBtn::Init()
{
	if (CurClickedType == EBtnClickedType::Highlight || CurClickedType == EBtnClickedType::Switch || CurClickedType == EBtnClickedType::Close)
	{
		ActionBtn->OnHovered.AddDynamic(this, &UStateMachineBtn::OnHovered);
		ActionBtn->OnUnhovered.AddDynamic(this, &UStateMachineBtn::OnUnhovered);
	}
	else if (CurClickedType == EBtnClickedType::Freedom)
	{
		ActionBtn->OnPressed.AddDynamic(this, &UStateMachineBtn::OnPressed);
		ActionBtn->OnReleased.AddDynamic(this, &UStateMachineBtn::OnReleased);
		ActionBtn->OnHovered.AddDynamic(this, &UStateMachineBtn::OnHovered);
		ActionBtn->OnUnhovered.AddDynamic(this, &UStateMachineBtn::OnUnhovered);
	}

	ActionBtn->OnClicked.AddDynamic(this, &UStateMachineBtn::OnClicked);
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
				if (const EBtnStateType* NewState = ActionMap->ActionWithState.Find(NewAction); NewState)
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

void UStateMachineBtn::OnClicked()
{
	SetAction(EBtnActionType::ClickedAction);
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

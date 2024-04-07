// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/Overlay.h"

#include "WidgetDefinition.h"
#include "StateMachineBtnDataAsset.h"

#include "StateMachineBtn.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class GENERALWIDGET_API UStateMachineBtn : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable)
	void Init();

	UFUNCTION(BlueprintCallable)
	FStateMachineInfo GetDataAsset() const;

	UFUNCTION(BlueprintCallable)
	EBtnStateType GetStateFromDataAsset(EBtnActionType NewAction, bool& bIsContainer) const;

	UFUNCTION(BlueprintCallable)
	void OnPressed();

	UFUNCTION(BlueprintCallable)
	void OnReleased();

	UFUNCTION(BlueprintCallable)
	void OnHovered();

	UFUNCTION(BlueprintCallable)
	void OnUnhovered();

	UFUNCTION(BlueprintCallable)
	void OnClicked();

	UFUNCTION(BlueprintCallable)
	void SetState(EBtnStateType NewState);

	UFUNCTION(BlueprintCallable)
	void SetAction(EBtnActionType NewAction);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool CheckIsTransitionAction();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	EBtnStateType TransitionState(EBtnActionType NewAction);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Switch Machine|State Data")
	UButton* ActionBtn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Switch Machine|State Data")
	EBtnStateType CurStateType = EBtnStateType::Start;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Switch Machine|State Data")
	EBtnActionType CurActionType = EBtnActionType::NoneAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Switch Machine|State Data", meta=(ExposeOnSpawn=true))
	EBtnClickedType CurClickedType = EBtnClickedType::NoHoverSwitch;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Switch Machine|State Map")
	TMap<EBtnStateType, int> StateIndexMap;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Switch Machine|State Map")
	TMap<int, UUserWidget*> IndexChildMap;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Switch Machine|State Data")
	UStateMachineBtnDataAsset* DA_StateMachine;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Switch Machine|State Data", meta=(ExposeOnSpawn=true))
	bool bIsUseDefaultContent = true;
};

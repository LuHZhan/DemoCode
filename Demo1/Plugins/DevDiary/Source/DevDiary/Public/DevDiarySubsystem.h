// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
// #include "UObject/UnrealTypePrivate.h"
#include "UObject/UnrealType.h"
#include "DevDiarySubsystem.generated.h"

/**
 * 
 */

class PureCpp
{
public:
	FString PrintPureCpp(FString& Input)
	{
		return Input;
	};
};


// 静态单播
DECLARE_DELEGATE_RetVal_OneParam(FString, FUnicastRetValParam, FString&);
DECLARE_DELEGATE_OneParam(FUnicastParam, int);
// 静态多播
DECLARE_MULTICAST_DELEGATE_OneParam(FMulticastParam, int);
// 动态单播
DECLARE_DYNAMIC_DELEGATE_RetVal_OneParam(bool, FDynamicUnicastParam, int, Number);
// 动态多播
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDynamicMulticastParam, int, Number);
// 事件
DECLARE_EVENT_OneParam(ADelegateTest, FDelegateEvent, int32);
// 派生事件
DECLARE_DERIVED_EVENT(UDevDiarySubsystem, FDelegateEvent, FDerivedDelegateEvent);


UCLASS(Blueprintable)
class DEVDIARY_API UDevDiarySubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, CustomThunk, meta=(DispalyName="SortWithFindFunction", DefaultToSelf="Object", ArrayParm="CustomStruct", AdvancedDisplay = "Object"))
	void SortWithFindFunction(UPARAM(ref) TArray<int32>& CustomStruct, UObject* Object, UPARAM(ref) FName& FunctionName);
	void Generic_SortWithFindFunction(void* TargetArray, FArrayProperty* ArrayProp, UObject* OwnerObject, UFunction* SortRuleFunc);
	DECLARE_FUNCTION(execSortWithFindFunction)
	{
		Stack.MostRecentProperty = nullptr;
		Stack.StepCompiledIn<FArrayProperty>(nullptr);
		void* ArrayAAddr = Stack.MostRecentPropertyAddress;
		FArrayProperty* ArrayProperty = static_cast<FArrayProperty*>(Stack.MostRecentProperty);
		if (ArrayProperty == nullptr)
		{
			Stack.bArrayContextFailed = true;
			return;
		}

		P_GET_OBJECT(UObject, OwnerObject);
		P_GET_PROPERTY(FNameProperty, FunctionName);

		if (OwnerObject == nullptr)
		{
			return;
		}
		UFunction* const Func = OwnerObject->FindFunction(FunctionName);
		if ((Func == nullptr || (Func->NumParms != 3)))
		{
			return;
		}

		P_FINISH;
		P_NATIVE_BEGIN;
			UDevDiarySubsystem* SubPtr = OwnerObject->GetWorld()->GetSubsystem<UDevDiarySubsystem>();
			SubPtr->Generic_SortWithFindFunction(ArrayAAddr, ArrayProperty, OwnerObject, Func);
		P_NATIVE_END
	}

	void ExecuteDelegate();

public:
	PureCpp Cpp1;

	// 静态单播
	FUnicastRetValParam OnUnicastRetValParam;
	// 静态单播
	FUnicastParam OnUnicastParam;
	// 静态多播
	FMulticastParam OnMulticastParam;
	// 动态单播
	FDynamicUnicastParam OnDynamicUnicastParam;
	// 动态多播
	FDynamicMulticastParam OnDynamicMulticastParam;
	// 事件
	FDelegateEvent OnDelegateEvent;

	static FString OnUnicastFunc(FString& Input);
	UFUNCTION(BlueprintCallable)
	FString UnicastUFUNC(FString& Input);
	FString MemberUnicastFunc(FString& Input);
	UFUNCTION(BlueprintCallable)
	bool DynamicUnicastFunc(int Number);
	void DynamicMulticastFunc(int Number);

	void BindDelegate();
};

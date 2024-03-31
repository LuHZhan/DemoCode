// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Widget.h"
#include "UObject/Interface.h"
#include "TracedInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType, Blueprintable)
class UTracedInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class TRACE_API ITracedInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, Category="Data")
	FString GetObjectTraceElementName();

	UFUNCTION(BlueprintNativeEvent, Category="Data")
	TSubclassOf<UUserWidget> GetCustomUIStyle();

	UFUNCTION(BlueprintNativeEvent, Category="Data")
	TSubclassOf<UUserWidget> GetLimitUIStyle();
	
	UFUNCTION(BlueprintNativeEvent, Category="Event")
	void DispatchOpenTrace();

	UFUNCTION(BlueprintNativeEvent, Category="Event")
	void DispatchCloseTrace();

	UFUNCTION(BlueprintNativeEvent, Category="Event")
	void DispatchAddToSubsystem();

	UFUNCTION(BlueprintNativeEvent, Category="Event")
	bool GetIsOpenTrace();

};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TaskGraphActor.generated.h"

UCLASS(BlueprintType)
class MULTITHREADING_API ATaskGraphActor : public AActor
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void AsyncLoadTextFile(UPARAM(ref) FString& FilePath);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void OnFileLoaded(const FString& FileContent);
};

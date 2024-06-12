// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RunThread.h"
#include "UseRunnableActor.generated.h"


UCLASS()
class MULTITHREADING_API AUseRunnableActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AUseRunnableActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void CreateRunnable(UPARAM(ref) int& InTestThread, const FString& ThreadingName, const FName& FunctionName);

	UFUNCTION(BlueprintCallable)
	void Pause();

	UFUNCTION(BlueprintCallable)
	void Sleep(float SleepTime);

	FRunThread* ThreadPtr;
};

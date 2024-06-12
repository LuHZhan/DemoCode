// Fill out your copyright notice in the Description page of Project Settings.


#include "UseRunnableActor.h"

#include "RunThread.h"


// Sets default values
AUseRunnableActor::AUseRunnableActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AUseRunnableActor::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AUseRunnableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AUseRunnableActor::CreateRunnable(int& InTestThreadConflict, const FString& ThreadingName, const FName& FunctionName)
{
	ThreadPtr = FRunThread::CreateThreading(InTestThreadConflict, this, FunctionName != FName() ? this->FindFunction(FunctionName) : nullptr, ThreadingName);
}

void AUseRunnableActor::Pause()
{
	ThreadPtr->Pause();
}

void AUseRunnableActor::Sleep(float SleepTime)
{
	FPlatformProcess::Sleep(SleepTime);
}

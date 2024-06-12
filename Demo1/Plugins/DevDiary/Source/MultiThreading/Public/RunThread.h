// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
// #include "GameFramework/Actor.h"
// #include "RunThread.generated.h"

class MULTITHREADING_API FRunThread : public FRunnable
{
public:
	FRunThread(int& InTestThreadConflict, UObject* Object, UFunction* InFunction, const FString& InThreadName);
	~FRunThread();

	// Begin FRunnable interface
	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Stop() override;
	virtual void Exit() override;
	// End FRunnable interface

	void Pause();
	void WeakUp();
	void EnsureCompletion();

	static FRunThread* CreateThreading(int& InTestThreadConflict,UObject* Object, UFunction* InFunction, const FString& InThreadName);
	static bool Suspend(const FString& InThreadName, bool bSuspend, bool bUseSuspend = true);
	static bool IsThreadingFinished(const FString& InThreadName, bool& bIsCompletion);
	static bool Shutdown(const FString& InThreadName);
	static FRunThread* GetThreading(const FString& InThreadName);

private:
	UObject* ObjectPtr;
	UFunction* FunctionPtr;
	FRunnableThread* ThreadingPtr;
	FString ThreadName;
	FEvent* EventPtr;
	int& TestThreadConflict;
	bool bPaused = false;
	bool bRunning = false;
	bool bCompletion = false;
	inline static TMap<FString, FRunThread*> ThreadingMap = {}; // <ThreadName, FRunThread>
};

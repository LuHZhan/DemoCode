// Fill out your copyright notice in the Description page of Project Settings.


#include "RunThread.h"

FRunThread::FRunThread(int& InTestThreadConflict, UObject* InObject, UFunction* InFunction, const FString& InThreadName): ObjectPtr(InObject), FunctionPtr(InFunction), ThreadName(InThreadName),
                                                                                                                          TestThreadConflict(InTestThreadConflict)
{
	EventPtr = FPlatformProcess::GetSynchEventFromPool();
	ThreadingPtr = FRunnableThread::Create(this, *ThreadName, 0, TPri_BelowNormal);
	bRunning = true;
}

FRunThread::~FRunThread()
{
	delete EventPtr;
	EventPtr = nullptr;
	delete ThreadingPtr;
	ThreadingPtr = nullptr;
	ObjectPtr = nullptr;
}

bool FRunThread::Init()
{
	return FRunnable::Init();
}

uint32 FRunThread::Run()
{
	//线程中使用Sleep来延迟逻辑执行时机
	FPlatformProcess::Sleep(0.03);

	static FCriticalSection Mutex;
	
	while (bRunning && TestThreadConflict < 200)
	{
		/*
		*使用bool值 bPause作为线程挂起的条件
		*bPause=true时,在Run函数中挂起线程,注意,要在本线程中挂起才可以!!!
		*/
		if (bPaused)
		{
			/*
			*一个线程实例对应一个FEvent指针,只有自己的FEvent指针能把自己挂起!!
			*当时用FEvent指针控制线程挂起时,一定要在想要挂起的线程中执行Wait()
			*因为Wait函数的特性为,将执行它的线程挂起,也就是说,如果在主线程中执行Wait,住线程将会被挂起
			*这会导致主线程看上去像是卡死了!!
			*/
			EventPtr->Wait();
			break;
		}

		if (FunctionPtr != nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("Run: %s"), *ThreadName);
			ObjectPtr->ProcessEvent(FunctionPtr, nullptr);
		}

		// static FCriticalSection Mutex;
		// FScopeLock ScopeLock(&Mutex);
		Mutex.Lock();
		TestThreadConflict++;
		Mutex.Unlock();

		//这里Sleep 一秒是为了限制while的执行频率,减少资源损耗
		FPlatformProcess::Sleep(0.05f);
		
	}

	UE_LOG(LogTemp, Warning, TEXT("RunThreadConflict: %s"), *FString::FromInt(TestThreadConflict));

	//while执行完成之后,Run函数返回,这代表着线程逻辑执行完成
	return 0;
}

void FRunThread::Stop()
{
	FRunnable::Stop();
	bRunning = false;
}

void FRunThread::Exit()
{
	FRunnable::Exit();
}

void FRunThread::Pause()
{
	bPaused = true;
	bRunning = false;
}

void FRunThread::WeakUp()
{
	bPaused = false;
	EventPtr->Trigger();
}

void FRunThread::EnsureCompletion()
{
	Stop();
	ThreadingPtr->WaitForCompletion();
	bCompletion = true;
}

FRunThread* FRunThread::CreateThreading(int& InTestThreadConflict, UObject* Object, UFunction* InFunction, const FString& InThreadName)
{
	if (FPlatformProcess::SupportsMultithreading())
	{
		FRunThread* Threading = new FRunThread(InTestThreadConflict, Object, InFunction, InThreadName);
		ThreadingMap.Add(InThreadName, Threading);
		return Threading;
	}
	else return nullptr;
}

bool FRunThread::Suspend(const FString& InThreadName, bool bSuspend, bool bUseSuspend)
{
	if (ThreadingMap.Find(InThreadName))
	{
		if (bUseSuspend)
		{
			/*线程激活/挂起方式一:
			*该种方式可以直接通过FRunnableThread::Suspend(true/false)的方式挂起或者激活线程
			*/
			ThreadingMap[InThreadName]->ThreadingPtr->Suspend(bSuspend);
		}
		else
		{
			/*线程激活/挂起方式二:
			*/
			if (bSuspend)
			{
				ThreadingMap[InThreadName]->Pause();
			}
			else
			{
				ThreadingMap[InThreadName]->WeakUp();
			}
		}
		return true;
	}
	return false;
}

bool FRunThread::IsThreadingFinished(const FString& InThreadName, bool& bIsCompletion)
{
	if (ThreadingMap.Find(InThreadName))
	{
		return ThreadingMap[InThreadName]->bCompletion;
	}
	return false;
}

bool FRunThread::Shutdown(const FString& InThreadName)
{
	if (ThreadingMap.Find(InThreadName))
	{
		ThreadingMap[InThreadName]->EnsureCompletion();
		ThreadingMap.Remove(InThreadName);
		return true;
	}
	return false;
}

FRunThread* FRunThread::GetThreading(const FString& InThreadName)
{
	if (ThreadingMap.Find(InThreadName))
	{
		return ThreadingMap[InThreadName];
	}
	return nullptr;
}

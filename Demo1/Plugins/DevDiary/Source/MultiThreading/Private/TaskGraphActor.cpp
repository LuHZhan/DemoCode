// Fill out your copyright notice in the Description page of Project Settings.


#include "TaskGraphActor.h"
#include "Misc/Paths.h"
#include "Misc/FileHelper.h"

DECLARE_DELEGATE_OneParam(FAsyncTaskLoadFileDelegate, FString);

class FTaskCompletionLoadFileToString
{
private:
	FAsyncTaskLoadFileDelegate TaskDelegate;
	FString FileContent;

public:
	FTaskCompletionLoadFileToString(const FAsyncTaskLoadFileDelegate& InTaskDelegate, FString& InFileContent)
		: TaskDelegate(InTaskDelegate), FileContent(InFileContent)
	{
	}

	// ~FTaskCompletionLoadFileToString();

	FORCEINLINE TStatId GetStatId() const
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(FTaskCompletionLoadFileToString, STATGROUP_TaskGraphTasks);
	}

	static ENamedThreads::Type GetDesiredThread()
	{
		return ENamedThreads::GameThread;
	}

	static ESubsequentsMode::Type GetSubsequentsMode()
	{
		return ESubsequentsMode::TrackSubsequents;
	}

	void DoTask(ENamedThreads::Type CurrentThread, const FGraphEventRef& MyCompletionGraphEvent)
	{
		check(IsInGameThread());
		TaskDelegate.Execute(FileContent);
	}
};

FAutoConsoleTaskPriority CPriorityLoadFileToString(
	TEXT("TaskGraph.TaskPriorities.LoadFileToString"),
	TEXT("Task and thread priority for file loading."),
	ENamedThreads::HighThreadPriority,
	ENamedThreads::NormalTaskPriority,
	ENamedThreads::HighTaskPriority
);

class FTaskLoadFileToString
{
private:
	FAsyncTaskLoadFileDelegate TaskDelegate;
	FString FilePath;
	FString FileContent;

public:
	FTaskLoadFileToString(FString& InFilePath, const FAsyncTaskLoadFileDelegate& InTaskDelegate) :
		TaskDelegate(InTaskDelegate), FilePath(MoveTemp(InFilePath)), FileContent(TEXT(""))
	{
	}
	// ~FTaskLoadFileToString();

	FORCEINLINE TStatId GetStatId() const
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(FTask_LoadFileToString, STATGROUP_TaskGraphTasks);
	}

	static ENamedThreads::Type GetDesiredThread()
	{
		return CPriorityLoadFileToString.Get();
	}

	static ESubsequentsMode::Type GetSubsequentsMode()
	{
		return ESubsequentsMode::TrackSubsequents;
	}

	void DoTask(ENamedThreads::Type CurrentThread, const FGraphEventRef& MyCompletionGraphEvent)
	{
		if (const FString FullPath = FPaths::Combine(FPaths::ProjectConfigDir(), FilePath); FPaths::FileExists(FullPath))
		{
			FFileHelper::LoadFileToString(FileContent, *FullPath);
		}

		FGraphEventRef ChildTask = TGraphTask<FTaskCompletionLoadFileToString>::CreateTask(nullptr, CurrentThread).ConstructAndDispatchWhenReady(TaskDelegate, FileContent);
		// MyCompletionGraphEvent->SetGatherThreadForDontCompleteUntil(ENamedThreads::GameThread);
		// 当前线程的完成被延迟，直到ChildTask完成为止
		MyCompletionGraphEvent->DontCompleteUntil(ChildTask);
	}
};


void ATaskGraphActor::AsyncLoadTextFile(FString& FilePath)
{
	FAsyncTaskLoadFileDelegate TaskDelegate;
	TaskDelegate.BindUFunction(this, "OnFileLoaded");

	TGraphTask<FTaskLoadFileToString>::CreateTask(nullptr, ENamedThreads::GameThread).ConstructAndDispatchWhenReady(FilePath, TaskDelegate);
}

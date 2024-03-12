// Fill out your copyright notice in the Description page of Project Settings.


#include "TraceSubsystem.h"
#include "TracedInterface.h"

#include "EngineUtils.h"
#include "Blueprint/GameViewportSubsystem.h"

void UTraceSubsystem::TraceSetting(FTraceSettingInfo NewTraceModule)
{
	TraceModule = NewTraceModule;
}

void UTraceSubsystem::Start()
{
	if (!bIsTracing)
	{
		bIsTracing = true;
	}
}

void UTraceSubsystem::End()
{
	if (bIsTracing)
	{
		bIsTracing = false;
	}
}


void UTraceSubsystem::Update(float DeltaTime)
{
	if (TraceModule.CurTraceModule == ETraceModule::DisplayedUI)
	{
		for (TTuple<FString, TWeakObjectPtr<AActor>> It : TraceObjectMap)
		{
			const FVector ObjectLocation = It.Value.Get()->GetActorLocation();
			const FVector CameraLocation = TraceModule.ViewportCamera.Get()->GetComponentLocation();
			const float FOVSize = TraceModule.ViewportCamera.Get()->FieldOfView;

			float ArcoDegrees;
			if (WorldLocationIsExistViewport(ObjectLocation, CameraLocation, FOVSize, TraceModule.ViewportCamera.Get()->GetForwardVector(), ArcoDegrees))
			{
				if (GetWorld())
				{
					const TWeakObjectPtr<ASceneUIActor>* UIActor = FindOrCreateUIActor(It, ObjectLocation);
					UIActor->Get()->SetVisible(true, false);
				}
			}
			else
			{
				if (TraceUIActorMap.Contains(It.Value))
				{
					TraceUIActorMap.Find(It.Value)->Get()->SetVisible(false, false);
				}
				if (!TraceUIWidgetMap.Contains(It.Value))
				{
					CreateUIToViewport(It);
				}
				MoveUIWidget(It);
			}
		}
	}
}

void UTraceSubsystem::Clear()
{
}

TWeakObjectPtr<ASceneUIActor>* UTraceSubsystem::FindOrCreateUIActor(const TTuple<FString, TWeakObjectPtr<AActor>>& PairIt, const FVector& ObjectLocation)
{
	if (!TraceUIActorMap.Contains(PairIt.Value))
	{
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnParameters.Owner = PairIt.Value.Get();

		ASceneUIActor* UIActor = GetWorld()->SpawnActor<ASceneUIActor>(ObjectLocation, FRotator::ZeroRotator, SpawnParameters);
		UIActor->UIName = PairIt.Key;
		UIActor->SetVisible(true, false);

		TraceUIActorMap.Add(PairIt.Value, TWeakObjectPtr<ASceneUIActor>(UIActor));
		ITracedInterface::Execute_DispatchOpenTrace(PairIt.Value.Get());
	}
	return TraceUIActorMap.Find(PairIt.Value);
}

void UTraceSubsystem::CreateUIToViewport(const TTuple<FString, TWeakObjectPtr<AActor>>& It)
{
	const TSubclassOf<UUserWidget> WidgetClass = ITracedInterface::Execute_GetCustomUIStyle(It.Value.Get());
	// if (UUserWidget* UIPtr = CreateWidget(It.Value.Get(), WidgetClass))
	// {
	// 	TraceUIWidgetMap.Add(It.Value.Get(), TWeakObjectPtr<UUserWidget>(UIPtr));
	// }
}

void UTraceSubsystem::MoveUIWidget(const TTuple<FString, TWeakObjectPtr<AActor>>& It)
{
	if (!TraceUIWidgetMap.Contains(It.Value))
		return;
	FVector ObjectLocation = It.Value.Get()->GetActorLocation();
	TWeakObjectPtr<UUserWidget>* UIPrt = TraceUIWidgetMap.Find(It.Value);
	if (!UIPrt->Get()->IsInViewport())
	{
		UIPrt->Get()->AddToViewport(TraceModule.UIZOrder);
	}

	
}


bool UTraceSubsystem::WorldLocationIsExistViewport(FVector ObjectLocation, FVector CameraLocation, const float FOVSize, FVector CameraForward,
                                                   float& ArcoDegrees) const
{
	const FVector CameraAtObject = ObjectLocation - CameraLocation;

	const float TargetCosValue = (CameraAtObject.Dot(CameraForward)) / (CameraAtObject.Length() * CameraForward.Length());
	const float PawnFOVCosValue = FMath::Cos(UE_DOUBLE_PI / (180) * (FOVSize / 2));

	ArcoDegrees = FMath::Acos(TargetCosValue) * 180 / UE_DOUBLE_PI;
	return TargetCosValue > PawnFOVCosValue;
}

void UTraceSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bIsTracing && CharacterIsMoveOrRotate())
	{
		Update(DeltaTime);
	}
}

TStatId UTraceSubsystem::GetStatId() const
{
	return TStatId();
}

bool UTraceSubsystem::TraceInit()
{
	TraceObjectMap.Empty();
	if (const UWorld* CurWorld = GetWorld())
	{
		for (FActorIterator It(CurWorld); It; ++It)
		{
			if ((*It)->StaticClass()->ImplementsInterface(UTracedInterface::StaticClass()))
			{
				TraceObjectMap.FindOrAdd(ITracedInterface::Execute_GetObjectTraceElementName(*It), TWeakObjectPtr<AActor>(*It));
				ITracedInterface::Execute_DispatchAddToSubsystem(*It);
			}
		}
		return true;
	}
	return false;
}

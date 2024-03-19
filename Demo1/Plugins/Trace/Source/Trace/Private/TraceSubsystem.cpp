// Fill out your copyright notice in the Description page of Project Settings.


#include "TraceSubsystem.h"
#include "TracedInterface.h"

#include "EngineUtils.h"
#include "Blueprint/GameViewportSubsystem.h"
#include "Kismet/GameplayStatics.h"

UTraceSubsystem::UTraceSubsystem()
{
}

void UTraceSubsystem::TraceSetting(FTraceSettingInfo NewTraceModule, FString ProjectFunctionName)
{
	TraceModule = NewTraceModule;
	if (ProjectFunctionName == "VerticalRectangle")
	{
		const FVector2D Size = UWidgetLayoutLibrary::GetViewportSize(GetWorld());
		TraceModule.ProjectFuncPtr = MakeShareable(new VerticalRectanglePF(Size.X, Size.Y, 10000.0f));
	}
	bIsNeedfulSetting = true;
}

void UTraceSubsystem::Start()
{
	if (!bIsTracing)
	{
		if (TraceInit() && bIsNeedfulSetting)
		{
			if (!ViewportWidgetWeakPtr.Get())
			{
				ViewportWidgetWeakPtr = CreateWidget<UTraceUIViewport>(GetWorld(), TraceModule.ViewportContainerClass);
				ViewportWidgetWeakPtr->AddToViewport();
			}

			bIsTracing = true;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Trace Start Failed,Because TraceSetting is not called"));
		}
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
			// TODO:完整的实现应该不需要检测
			if (CheckWorldLocationIsExistViewport(ObjectLocation, CameraLocation, FOVSize, TraceModule.ViewportCamera.Get()->GetForwardVector(),
			                                      ArcoDegrees))
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
	if (UUserWidget* UIPtr = CreateWidget<UUserWidget>(GetWorld(), WidgetClass))
	{
		TraceUIWidgetMap.Add(It.Value.Get(), TWeakObjectPtr<UUserWidget>(UIPtr));
	}
}

void UTraceSubsystem::MoveUIWidget(const TTuple<FString, TWeakObjectPtr<AActor>>& It)
{
	if (!TraceUIWidgetMap.Contains(It.Value))
		return;

	const FVector ObjectLocation = It.Value.Get()->GetActorLocation();
	const TWeakObjectPtr<UUserWidget>* UIPrt = TraceUIWidgetMap.Find(It.Value);
	// if (!UIPrt->Get()->IsInViewport())
	// {
	// 	UIPrt->Get()->AddToViewport(TraceModule.UIZOrder);
	// }


	const FVector2D RealXY = GetProjectToScreen(Cast<APlayerController>(TraceModule.ViewportCharacter->GetController()), ObjectLocation);
	const FVector2D ViewportSize = UWidgetLayoutLibrary::GetViewportSize(GetWorld());
	TraceModule.ProjectFuncPtr->Update(ViewportSize.X, ViewportSize.Y);
	PointInfo XYInfo = TraceModule.ProjectFuncPtr->GetCrossLocation(RealXY.X, RealXY.Y);
}


FVector2D UTraceSubsystem::GetWorldLocationToScreenPosition(APlayerController* PlayerController, FVector WorldLocation, bool bPlayerViewportRelative,
                                                            bool& bProject) const
{
	FVector2D ViewportPosition;
	bProject = UWidgetLayoutLibrary::ProjectWorldLocationToWidgetPosition(PlayerController, WorldLocation, ViewportPosition, bPlayerViewportRelative);
	return ViewportPosition;
}

bool UTraceSubsystem::CheckWorldLocationIsExistViewport(FVector ObjectLocation, FVector CameraLocation, const float FOVSize, FVector CameraForward,
                                                        float& ArcoDegrees) const
{
	const FVector CameraAtObject = ObjectLocation - CameraLocation;

	const float TargetCosValue = (CameraAtObject.Dot(CameraForward)) / (CameraAtObject.Length() * CameraForward.Length());
	const float PawnFOVCosValue = FMath::Cos(UE_DOUBLE_PI / (180) * (FOVSize / 2));

	ArcoDegrees = FMath::Acos(TargetCosValue) * 180 / UE_DOUBLE_PI;
	return TargetCosValue > PawnFOVCosValue;
}

FVector2D UTraceSubsystem::GetProjectToScreen(APlayerController* PlayerController, FVector WorldLocation)
{
	FMatrix ViewMatrix;
	FMatrix ProjectionMatrix;
	FMatrix ViewProjectionMatrix;
	UGameplayStatics::GetViewProjectionMatrix(PlayerController->PlayerCameraManager->GetCameraCacheView(), ViewMatrix, ProjectionMatrix,
	                                          ViewProjectionMatrix);

	FVector2D Result(ViewMatrix.TransformFVector4(FVector4(WorldLocation, 1)).X, ViewMatrix.TransformFVector4(FVector4(WorldLocation, 1)).Y);
	UE_LOG(LogTemp, Warning, TEXT("X==%f,Y==%f"), Result.X, Result.Y);
	return Result;
}

FVector2D UTraceSubsystem::TestGetScreenPosition(APlayerController* PlayerController, FVector WorldLocation)
{
	const FVector2D RealXY = GetProjectToScreen(PlayerController, WorldLocation);

	const FVector2D ViewportSize = UWidgetLayoutLibrary::GetViewportSize(GetWorld()) / UWidgetLayoutLibrary::GetViewportScale(GetWorld()) *
		ProjectViewportScale;
	TraceModule.ProjectFuncPtr->Update(ViewportSize.X, ViewportSize.Y);
	const PointInfo XYInfo = TraceModule.ProjectFuncPtr->GetCrossLocation(RealXY.X, RealXY.Y);

	return FVector2D(XYInfo.NearestXY.first, XYInfo.NearestXY.second);
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

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
			// TODO:这里调用是为了防止进入游戏时因为人物没有移动或者旋转镜头而导致Update完全不触发
			Update(0.01);
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
			if (CheckWorldLocationIsExistViewport(ObjectLocation, CameraLocation, FOVSize * 0.7, TraceModule.ViewportCamera.Get()->GetForwardVector(),
			                                      ArcoDegrees))
			{
				// ToggleViewport(false);
				// if (GetWorld())
				// {
				// 	const TWeakObjectPtr<ASceneUIActor>* UIActor = FindOrCreateUIActor(It, ObjectLocation);
				// 	// TODO: 应该将当前是否显示在屏幕内作为一个变量去进行动态判断，发生变化再进行切换
				// 	// UIActor->Get()->SetVisible(true, true);
				//
				// 	ToggleViewport(true);
				// 	// if (TraceUIActorMap.Contains(It.Value))
				// 	// {
				// 	// 	TraceUIActorMap.Find(It.Value)->Get()->SetVisible(false, false);
				// 	// }
				// 	if (!TraceUIWidgetMap.Contains(It.Value))
				// 	{
				// 		CreateUIToViewport(It);
				// 	}
				// 	MoveUIWidget(It);
				// }

				ToggleViewport(true);
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
			else
			{
				ToggleViewport(true);
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
		UIActor->UIClass = ITracedInterface::Execute_GetCustomUIStyle(PairIt.Value.Get());
		UIActor->UpdateUIData();
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

	// 初始化ProjectFunc的边界数据
	const FVector2D ViewportSize = UWidgetLayoutLibrary::GetViewportSize(GetWorld()) / UWidgetLayoutLibrary::GetViewportScale(GetWorld()) *
		ProjectViewportScale;
	TraceModule.ProjectFuncPtr->Update(ViewportSize.X, ViewportSize.Y);
	const FVector2D Range = GetProjectCoordinateLimit();

	// 执行投影，得到初始的YX坐标
	FVector2D RealXY(0, 0);
	bool bIsRequireCross = false;
	GetProjectToScreen(Cast<APlayerController>(TraceModule.ViewportCharacter->GetController()),
	                   ObjectLocation,
	                   Range, bIsRequireCross, RealXY
	);

	PointInfo XYInfo = bIsRequireCross
		                   ? TraceModule.ProjectFuncPtr->GetCrossLocation(RealXY.X, RealXY.Y)
		                   : PointInfo(true,
		                               TArray<TPair<float, float>>({{RealXY.X, RealXY.Y}}),
		                               FVector2D(RealXY.X, RealXY.Y));

	// UE_LOG(LogTemp, Warning, TEXT("XYInfo.NearestXY.first==%f,XYInfo.NearestXY.second==%f"), XYInfo.NearestXY.first, XYInfo.NearestXY.second);
	ViewportWidgetWeakPtr->AddOrUpdateUI(It.Key, *UIPrt, {XYInfo.NearestXY.first, XYInfo.NearestXY.second});
}

void UTraceSubsystem::ToggleViewport(bool NewAnchoring)
{
	if (ViewportWidgetWeakPtr->bIsStartAnchoring != NewAnchoring)
	{
		ViewportWidgetWeakPtr->Toggle(NewAnchoring);
	}
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

bool UTraceSubsystem::CheckCoordinateIsExistRange(FVector2D Target, FVector2D Range, bool bIsEqual /*= false*/)
{
	if (bIsEqual)
	{
		return Target.X >= Range.X * -1 && Target.X <= Range.X && Target.Y >= Range.Y * -1 && Target.Y <= Range.Y;
	}
	return Target.X >= Range.X * -1 && Target.X <= Range.X && Target.Y >= Range.Y * -1 && Target.Y <= Range.Y;
}

FVector2D UTraceSubsystem::ClampCoordinate(FVector2D Target, FVector2D Range) const
{
	FVector2D Result = {FMath::Clamp(Target.X, Range.X * -1, Range.X), FMath::Clamp(Target.Y, Range.Y * -1, Range.Y)};
	return Result;
}

FVector2D UTraceSubsystem::GetClosestLimitCoordinate(FVector2D Target, FVector2D Range) const
{
	FVector2d Limit;
	Limit.X = FMath::Abs(Target.X - Range.X) <= FMath::Abs(Target.X - (Range.X * -1)) ? Range.X : Range.X * -1;
	Limit.Y = FMath::Abs(Target.Y - Range.Y) <= FMath::Abs(Target.Y - (Range.Y * -1)) ? Range.Y : Range.Y * -1;

	FVector2D Percent;
	Percent.X = Target.X / Limit.X;
	Percent.Y = Target.Y / Limit.Y;

	return Percent.X >= Percent.Y ? FVector2D({Limit.X, Target.Y}) : FVector2D({Target.X, Limit.Y});
}

FVector2D UTraceSubsystem::GetProjectCoordinateLimit() const
{
	if (TraceModule.ProjectFuncPtr)
	{
		return {TraceModule.ProjectFuncPtr->Width / 2, TraceModule.ProjectFuncPtr->Height / 2};
	}
	return FVector2D(0, 0);
}

void UTraceSubsystem::GetProjectToScreen(APlayerController* PlayerController, FVector WorldLocation, FVector2D Range, bool& bIsRequireCross,
                                         FVector2D& Result)
{
	float ArcoDegrees;
	if (CheckWorldLocationIsExistViewport(WorldLocation, TraceModule.ViewportCamera.Get()->GetComponentLocation(),
	                                      TraceModule.ViewportCamera->FieldOfView,
	                                      TraceModule.ViewportCamera->GetForwardVector(), ArcoDegrees))
	{
		// TODO: 全程使用ProjectWorldLocationToWidgetPosition不可行，处于身后的物体不能够正确的显示位置
		UWidgetLayoutLibrary::ProjectWorldLocationToWidgetPosition(PlayerController, WorldLocation, Result, false);

		Result.X = Result.X - UWidgetLayoutLibrary::GetViewportSize(GetWorld()).X / UWidgetLayoutLibrary::GetViewportScale(GetWorld()) / 2;
		// 这里是为了适配用视图矩阵实现的坐标，有统一的通道
		Result.Y = (Result.Y - UWidgetLayoutLibrary::GetViewportSize(GetWorld()).Y / UWidgetLayoutLibrary::GetViewportScale(GetWorld()) / 2) * -1;

		Result = CheckCoordinateIsExistRange(Result, Range) ? Result : ClampCoordinate(Result, Range);
		bIsRequireCross = false;
		return;
	}

	// TODO: 对于视野外视图矩阵不完全对，会出现值过小的问题，同时会出现y值跟Project不符
	FMatrix ViewMatrix;
	FMatrix ProjectionMatrix;
	FMatrix ViewProjectionMatrix;
	UGameplayStatics::GetViewProjectionMatrix(PlayerController->PlayerCameraManager->GetCameraCacheView(), ViewMatrix, ProjectionMatrix,
	                                          ViewProjectionMatrix);

	const FVector4 TransformVec = ViewMatrix.TransformFVector4(FVector4(WorldLocation, 1));
	Result = FVector2D(TransformVec.X, TransformVec.Y);

	// Result = GetClosestLimitCoordinate(Result, Range);
	// UE_LOG(LogTemp, Warning, TEXT("NotRange::X==%f,Y==%f,Z==%f"), MatrixVector4.X, MatrixVector4.Y, MatrixVector4.Z);
	UKismetSystemLibrary::PrintString(GetWorld(),
	                                  FString::Printf(TEXT("NotRange::X==%f,Y==%f,Z==%f"), TransformVec.X, TransformVec.Y, TransformVec.Z));
	bIsRequireCross = true;
}

FVector2D UTraceSubsystem::TestGetScreenPosition(APlayerController* PlayerController, FVector WorldLocation)
{
	bool a;
	FVector2D RealXY;
	GetProjectToScreen(PlayerController, WorldLocation, GetProjectCoordinateLimit(), a, RealXY);

	const FVector2D ViewportSize = UWidgetLayoutLibrary::GetViewportSize(GetWorld()) / UWidgetLayoutLibrary::GetViewportScale(GetWorld()) *
		ProjectViewportScale;
	TraceModule.ProjectFuncPtr->Update(ViewportSize.X, ViewportSize.Y);
	const PointInfo XYInfo = TraceModule.ProjectFuncPtr->GetCrossLocation(RealXY.X, RealXY.Y);

	return FVector2D(XYInfo.NearestXY.first, XYInfo.NearestXY.second);
}

void UTraceSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bIsTracing /*&& CharacterIsMoveOrRotate()*/)
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
			if ((*It)->GetClass()->ImplementsInterface(UTracedInterface::StaticClass()))
			{
				TraceObjectMap.FindOrAdd(ITracedInterface::Execute_GetObjectTraceElementName(*It), TWeakObjectPtr<AActor>(*It));
				ITracedInterface::Execute_DispatchAddToSubsystem(*It);
			}
		}
		return true;
	}
	return false;
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "TraceSubsystem.h"
#include "TracedInterface.h"

#include "EngineUtils.h"
#include "Blueprint/GameViewportSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

UTraceSubsystem::UTraceSubsystem()
{
}

void UTraceSubsystem::TraceSetting(FTraceSettingInfo NewTraceModule, FString ProjectFunctionName)
{
	TraceModule = NewTraceModule;
	if (ProjectFunctionName == "VerticalRectangle")
	{
		// TODO: 这里的ViewportSize在LevelBlueprint不能够正常运行
		const FVector2D ViewportSize = UWidgetLayoutLibrary::GetViewportSize(GetWorld()) / UWidgetLayoutLibrary::GetViewportScale(GetWorld()) *
			ProjectViewportScale;
		TraceModule.ProjectFuncPtr = MakeShareable(new VerticalRectanglePF(ViewportSize.X, ViewportSize.Y, 10000.0f));
		TraceModule.ProjectFuncPtr->UpdateRange(ViewportSize.X, ViewportSize.Y);
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
			ToggleViewport(true);
			if (!TraceUIWidgetMap.Contains(It.Value))
			{
				CreateUIToViewport(It);
			}
			MoveUIWidget(It);
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

bool UTraceSubsystem::CreateUIToViewport(const TTuple<FString, TWeakObjectPtr<AActor>>& It)
{
	const TSubclassOf<UUserWidget> NormalWidgetClass = ITracedInterface::Execute_GetCustomUIStyle(It.Value.Get());
	const TSubclassOf<UUserWidget> LimitWidgetClass = ITracedInterface::Execute_GetLimitUIStyle(It.Value.Get());
	if (UUserWidget* NormalUIPtr = CreateWidget<UUserWidget>(GetWorld(), NormalWidgetClass); NormalUIPtr)
	{
		TraceUIWidgetMap.Add(It.Value.Get(), {});
		TraceUIWidgetMap.Find(It.Value.Get())->Add(TWeakObjectPtr<UUserWidget>(NormalUIPtr));
		if (UUserWidget* LimitUIPtr = CreateWidget<UUserWidget>(GetWorld(), LimitWidgetClass); LimitUIPtr)
		{
			TraceUIWidgetMap.Find(It.Value.Get())->Add(TWeakObjectPtr<UUserWidget>(LimitUIPtr));
			return true;
		}
		// 两个UI都使用同一种
		TraceUIWidgetMap.Find(It.Value.Get())->Add(TWeakObjectPtr<UUserWidget>(NormalUIPtr));
		return true;
	}
	return false;
}

void UTraceSubsystem::MoveUIWidget(const TTuple<FString, TWeakObjectPtr<AActor>>& It)
{
	if (!TraceUIWidgetMap.Contains(It.Value))
		return;

	const FVector ObjectLocation = It.Value.Get()->GetActorLocation();
	// const TWeakObjectPtr<UUserWidget>* UIPrt = &(*(TraceUIWidgetMap.Find(It.Value)))[0];

	// 获取边界函数的坐标的限制范围
	const FVector2D Range = GetProjectCoordinateLimit();

	// 执行投影，得到初始的YX坐标
	FVector2D RealXY(0, 0);
	bool bIsRequireCross = false;
	GetProjectToScreen(Cast<APlayerController>(TraceModule.ViewportCharacter->GetController()), ObjectLocation, RealXY
	);

	// 得到跟边界函数的交点
	PointInfo XYInfo = TraceModule.ProjectFuncPtr->GetCrossLocation(RealXY.X, RealXY.Y);

	// Output
	// UKismetSystemLibrary::PrintString(GetWorld(),
	//                                   FString::Printf(TEXT("GetCrossLocation::X==%f,Y==%f"), XYInfo.NearestXY.first, XYInfo.NearestXY.second));

	EUIStyleType CurStyleType;
	FVector4 Data(XYInfo.NearestXY.first, XYInfo.NearestXY.second, 0, 0);
	GetSelectedUIStyle(It.Key, {XYInfo.NearestXY.first, XYInfo.NearestXY.second}, CurStyleType, Data);

	// 将值交给ViewportWidget
	ViewportWidgetWeakPtr->AddOrUpdateUI(It.Key, (*(TraceUIWidgetMap.Find(It.Value)))[0], (*(TraceUIWidgetMap.Find(It.Value)))[1],
	                                     CurStyleType, Data);
	// ViewportWidgetWeakPtr->AddOrUpdateUI(It.Key, *UIPrt, EUIStyleType::Normal, Data);
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
	if (Target.X == 0 && Target.Y == 0)
		return FVector2D{0, 0};

	if (UKismetMathLibrary::InRange_FloatFloat(Target.X, Range.X * -1, Range.X) &&
		UKismetMathLibrary::InRange_FloatFloat(Target.Y, Range.Y * -1, Range.Y))
		return Target;

	// 求象限坐标
	FVector2d Limit;
	Limit.X = FMath::Abs(Target.X - Range.X) <= FMath::Abs(Target.X - (Range.X * -1)) ? Range.X : Range.X * -1;
	Limit.Y = FMath::Abs(Target.Y - Range.Y) <= FMath::Abs(Target.Y - (Range.Y * -1)) ? Range.Y : Range.Y * -1;


	// 求函数射线无限延申的情况下，会先于哪条边相交
	FVector2D Percent;
	Percent.X = Target.X / Limit.X;
	Percent.Y = Target.Y / Limit.Y;

	const float A = (Target.Y / Target.X);
	// 直接使用Target.XorY，如果Target的值超过了Range的范围，则会得到矩阵顶点的坐标，结果是不对的，所以使用A去计算，得到在矩阵边相交点
	return Percent.X >= Percent.Y ? FVector2D({Limit.X, Limit.X * A}) : FVector2D({Limit.Y / A, Limit.Y});
}

FVector2D UTraceSubsystem::GetProjectCoordinateLimit() const
{
	if (TraceModule.ProjectFuncPtr)
	{
		return {TraceModule.ProjectFuncPtr->Width / 2, TraceModule.ProjectFuncPtr->Height / 2};
	}
	return FVector2D(0, 0);
}

// void UTraceSubsystem::a_Implementation(FString TraceActorName, FVector2D Parameter2D)
// {
// 	
// }

void UTraceSubsystem::GetSelectedUIStyle(const FString TraceActorName, const FVector2D Parameter2D, EUIStyleType& StyleType, FVector4& Data) const
{
	if (const FVector2D Range = GetProjectCoordinateLimit(); Parameter2D.X == Range.X || Parameter2D.Y == Range.Y)
	{
		StyleType = EUIStyleType::Limit;
		const float ClockWiseAngle = 360 - FMath::Acos((FVector(0, 100, 0).Dot(FVector(Data.X, Data.Y, 0)))
			/ (100 * FVector(Data.X, Data.Y, 0).Length())) * 180 / UE_DOUBLE_PI;
		Data.Z = ClockWiseAngle < 180 ? ClockWiseAngle : ClockWiseAngle - 360;
		return;
	}

	StyleType = EUIStyleType::Normal;
}


void UTraceSubsystem::GetProjectToScreen(APlayerController* PlayerController, FVector WorldLocation,
                                         FVector2D& Result)
{
	FMatrix ViewMatrix;
	FMatrix ProjectionMatrix;
	FMatrix ViewProjectionMatrix;
	UGameplayStatics::GetViewProjectionMatrix(PlayerController->PlayerCameraManager->GetCameraCacheView(), ViewMatrix, ProjectionMatrix,
	                                          ViewProjectionMatrix);

	// 获取MVP矩阵之后屏幕内[-1,1]之间的比例
	const FVector4 TransformVec = ViewProjectionMatrix.TransformFVector4(FVector4(WorldLocation, 1));
	const float AbsW = UKismetMathLibrary::Abs(TransformVec.W);
	const FVector4 PerDivided = TransformVec / AbsW;

	// Output 
	// UKismetSystemLibrary::PrintString(GetWorld(),
	//                                   FString::Printf(TEXT("PerDivided::X==%f,Y==%f,Z==%f"), TransformVec.X / AbsW, TransformVec.Y / AbsW,
	//                                                   TransformVec.Z / TransformVec.W));

	// 将比例转化为具体的数值并返回
	const FVector2D ViewportSize = UWidgetLayoutLibrary::GetViewportSize(GetWorld()) / UWidgetLayoutLibrary::GetViewportScale(GetWorld()) / 2;
	Result.X = PerDivided.X * ViewportSize.X;
	Result.Y = PerDivided.Y * ViewportSize.Y;
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

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// #include <vector>
#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Actor.h"
#include "TraceDefinition.generated.h"

UCLASS()
class TRACE_API ATraceDefinition : public AActor
{
	GENERATED_BODY()
};

UENUM(Blueprintable, BlueprintType)
enum ETraceModule
{
	DisplayedUI = 0,
	DisplayedUIOffViewport,
	NotDisplayedUI,
	AutoTrace,
};

/* Function of project to viewport */

typedef std::vector<std::pair<float, float>> XYArray;

struct PointInfo
{
	bool IsValid;
	XYArray XYs;
	std::pair<float, float> NearestXY;
};

class BaseProjectFunctionType
{
public:
	virtual PointInfo GetProjectXY(float x, float y) const =0;
	virtual std::vector<float> Fx(float x) const =0;
	virtual void Update(const float NewWidth, const float NewHeight, const float NewNotValidNumber = 0.0f) =0;

	BaseProjectFunctionType(const float NewWidth, const float NewHeight, const float NewNotValidNumber = 0.0f): Width(NewWidth), Height(NewHeight),
		NotValidNumber(NewNotValidNumber)
	{
	};

	virtual ~BaseProjectFunctionType()
	{
	};

	float Width;
	float Height;
	float NotValidNumber;
};

class VerticalRectanglePF final : public BaseProjectFunctionType
{
public:
	VerticalRectanglePF(const float NewWidth, const float NewHeight, const float NewNotValidNumber = 0.0f): BaseProjectFunctionType(
		NewWidth, NewHeight, NewNotValidNumber)
	{
		XRange = {(NewWidth / 2) * -1, NewWidth / 2};
		YRange = {(NewHeight / 2) * -1, NewHeight / 2};
	};

	virtual void Update(const float NewWidth, const float NewHeight, const float NewNotValidNumber = 0.0f) override
	{
		Width = NewWidth;
		Height = NewHeight;
		NotValidNumber = NewNotValidNumber;

		XRange = {(NewWidth / 2) * -1, NewWidth / 2};
		YRange = {(NewHeight / 2) * -1, NewHeight / 2};
	}

	virtual std::vector<float> Fx(float x) const override
	{
		std::vector<float> Ys;
		if (std::clamp(x, XRange.first, XRange.second))
		{
			Ys.push_back(YRange.first);
			Ys.push_back(YRange.second);
		}
		else
		{
			Ys.push_back(NotValidNumber);
		}
		return Ys;
	}

	virtual PointInfo GetProjectXY(float x, float y) const override
	{
		PointInfo Result;
		if (x == 0 || y == 0)
		{
			Result.IsValid = false;
			return Result;
		}

		// y = ax 
		const float a = y / x;
		float MinusX = YRange.first / a;
		float PositiveX = YRange.second / a;
		Result.XYs.push_back({MinusX, YRange.first});
		Result.XYs.push_back({PositiveX, YRange.second});
		Result.NearestXY.first = y > 0 ? PositiveX : MinusX;
		Result.NearestXY.second = YRange.second;
		Result.IsValid = true;
		return Result;
	}

	std::pair<float, float> XRange;
	std::pair<float, float> YRange;
};

/* Function of project to viewport */

USTRUCT(Blueprintable, BlueprintType)
struct FTraceSettingInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="FTraceSettingInfo")
	TEnumAsByte<ETraceModule> CurTraceModule;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="FTraceSettingInfo")
	TWeakObjectPtr<ACharacter> ViewportCharacter;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="FTraceSettingInfo")
	TWeakObjectPtr<UCameraComponent> ViewportCamera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="FTraceSettingInfo")
	int32 UIZOrder = 10000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="FTraceSettingInfo")
	TSubclassOf<UUserWidget> ViewportContent;

	TSharedPtr<BaseProjectFunctionType> ProjectFuncPtr;

	FTraceSettingInfo(): CurTraceModule(ETraceModule::DisplayedUI), ViewportCharacter(nullptr), ViewportCamera(nullptr), ProjectFuncPtr(nullptr)
	{
	};
};

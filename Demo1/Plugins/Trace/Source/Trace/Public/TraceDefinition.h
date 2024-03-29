// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// #include <vector>
#include "CoreMinimal.h"
#include "map"
#include "Camera/CameraComponent.h"
#include "GameFramework/Actor.h"
#include "Kismet/KismetMathLibrary.h"
#include "TraceDefinition.generated.h"

UCLASS()
class TRACE_API ATraceDefinition : public AActor
{
	GENERATED_BODY()
};

// TODO: 目前只实现了DisplayedUI，后续应该针对每一个TraceActor实现不同的Module
UENUM(Blueprintable, BlueprintType)
enum class ETraceModule:uint8
{
	DisplayedUI = 0,
	DisplayedUIOffViewport,
	NotDisplayedUI,
	AutoTrace,
};

UENUM(Blueprintable, BlueprintType)
enum class EUIStyleType:uint8
{
	Normal = 0,
	Limit
};

/* Function of project to viewport */

typedef std::vector<std::pair<float, float>> XYArray;

enum Quadrant
{
	One = 0,
	Two,
	Three,
	Four
};

enum Direction
{
	Left = 0,
	Right,
	Up,
	Down
};

enum Axis
{
	X = 0,
	Y,
	Z,
	W,
};

struct PointInfo
{
	PointInfo(): IsValid(false), XYs({}), NearestXY({})
	{
	};

	PointInfo(bool NewIsValid, const XYArray& NewXYs, const std::pair<float, float>& NewNearestXY): IsValid(NewIsValid), XYs(NewXYs), NearestXY(NewNearestXY)
	{
	};

	PointInfo(bool NewIsValid, const TArray<TPair<float, float>>& NewXYs, const FVector2D& NewNearestXY): IsValid(NewIsValid),
		NearestXY(NewNearestXY.X, NewNearestXY.Y)
	{
		for (const TPair<float, float>& XY : NewXYs)
		{
			XYs.push_back({XY.Key, XY.Value});
		}
	};

	bool IsValid;
	XYArray XYs;
	std::pair<float, float> NearestXY;
};

class BaseProjectFunctionType
{
public:
	virtual PointInfo GetCrossLocation(float x, float y) const =0;
	virtual std::vector<float> Fx(float x) const =0;

	/**
	 * 
	 * @param x Vector X
	 * @param y Vector Y
	 * @return What quadrant does the coordinate lie in?
	 */
	static Quadrant GetQuadrant(float x, float y)
	{
		if (x > 0 && y > 0)
		{
			return One;
		}
		else if (x < 0 && y > 0)
		{
			return Two;
		}
		else if (x < 0 && y < 0)
		{
			return Three;
		}
		else return Four;
	}

	/**
	 * TODO:只实现了左右的区分，应该还有上下的区分
	 * @param x1 Target	Vector X
	 * @param y1 Target	Vector Y
	 * @param x2 Const	Vector X
	 * @param y2 Const	Vector Y
	 * @return Vector1 is at the position of Vector2
	 */
	static Direction GetDirection(float x1, float y1, const float x2, const float y2)
	{
		return (x1 * y2) - (x2 * y1) > 0 ? Direction::Right : Direction::Left;
	}

	virtual void UpdateRange(const float NewWidth, const float NewHeight, const float NewNotValidNumber = 0.0f) =0;

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

		QuadrantMap = {
			{Quadrant::One, {XRange.second, YRange.second}},
			{Quadrant::Two, {XRange.first, YRange.second}},
			{Quadrant::Three, {XRange.first, YRange.first}},
			{Quadrant::Four, {XRange.second, YRange.first}}
		};

		UnKnownAxisMap = {
			{{Quadrant::One, Direction::Left}, {Axis::X}},
			{{Quadrant::One, Direction::Right}, {Axis::Y}},
			{{Quadrant::Two, Direction::Left}, {Axis::Y}},
			{{Quadrant::Two, Direction::Right}, {Axis::X}},
			{{Quadrant::Three, Direction::Left}, {Axis::X}},
			{{Quadrant::Three, Direction::Right}, {Axis::Y}},
			{{Quadrant::Four, Direction::Left}, {Axis::Y}},
			{{Quadrant::Four, Direction::Right}, {Axis::X}}
		};
	};

	virtual void UpdateRange(const float NewWidth, const float NewHeight, const float NewNotValidNumber = 0.0f) override
	{
		Width = NewWidth;
		Height = NewHeight;
		NotValidNumber = NewNotValidNumber;

		XRange = {(NewWidth / 2) * -1, NewWidth / 2};
		YRange = {(NewHeight / 2) * -1, NewHeight / 2};

		QuadrantMap.clear();
		QuadrantMap = {
			{Quadrant::One, {XRange.second, YRange.second}},
			{Quadrant::Two, {XRange.first, YRange.second}},
			{Quadrant::Three, {XRange.first, YRange.first}},
			{Quadrant::Four, {XRange.second, YRange.first}}
		};
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

	virtual PointInfo GetCrossLocation(float x, float y) const override
	{
		PointInfo Result;
		if (x == 0 || y == 0)
		{
			Result.IsValid = false;
			return Result;
		}

		if (x >= XRange.first && x <= XRange.second && y >= YRange.first && y <= YRange.second)
		{
			Result.XYs.push_back({x, y});
			Result.NearestXY = {x, y};
			Result.IsValid = true;
			return Result;
		}

		const Quadrant Position = GetQuadrant(x, y);
		const float XMax = QuadrantMap.find(Position)->second.first;
		const float YMax = QuadrantMap.find(Position)->second.second;
		const Direction VectorDirection = GetDirection(x, y, XMax, YMax);

		const float a = y / x;
		if (UnKnownAxisMap.contains({Position, VectorDirection}))
		{
			const Axis CurAxis = UnKnownAxisMap.find({Position, VectorDirection})->second;
			// TODO:这里的写法可以优化
			if (CurAxis == Axis::X)
			{
				float XValue = YMax / a;
				Result.XYs.push_back({XValue, YMax});
				Result.XYs.push_back({XValue * -1, YMax * -1});
				Result.NearestXY = {XValue, YMax};
				Result.IsValid = true;
			}
			else if (CurAxis == Axis::Y)
			{
				float YValue = XMax * a;
				Result.XYs.push_back({XMax, YValue});
				Result.XYs.push_back({XMax * -1, YValue * -1});
				Result.NearestXY = {XMax, YValue};
				Result.IsValid = true;
			}
		}
		return Result;
	}

	std::pair<float, float> XRange;
	std::pair<float, float> YRange;
	std::map<Quadrant, std::pair<float, float>> QuadrantMap;
	std::map<std::pair<Quadrant, Direction>, Axis> UnKnownAxisMap;
};

// PointInfo Result;
// if (x == 0 || y == 0)
// {
// 	Result.IsValid = false;
// 	return Result;
// }
//
// if (x >= XRange.first && x <= XRange.second && y >= YRange.first && y <= YRange.second)
// {
// 	Result.XYs.push_back({x, y});
// 	Result.NearestXY = {x, y};
// 	Result.IsValid = true;
// 	return Result;
// }
//
// std::pair<float, float> Limit;
// Limit.first = abs(x - XRange.second) <= abs(x - (XRange.first)) ? XRange.second : XRange.first;
// Limit.second = abs(y - YRange.second) <= abs(y - (YRange.first)) ? YRange.second : YRange.first;
//
// std::pair<float, float> Percent;
// Percent.first = x / Limit.first;
// Percent.second = y / Limit.second;
//
// const float A = (y / x);
// // 直接使用Target.XorY，如果Target的值超过了Range的范围，则会得到矩阵顶点的坐标，结果是不对的，所以使用A去计算，得到在矩阵边相交点
// const std::pair<float, float> ResultVector2D = Percent.first >= Percent.second
// 	                                         ? std::pair<float, float>({Limit.first, Limit.first * A})
// 	                                         : std::pair<float, float>({Limit.second / A, Limit.second});
// Result.XYs.push_back(ResultVector2D);
// Result.NearestXY = ResultVector2D;
// Result.IsValid = true;
// return Result;

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
	TSubclassOf<UUserWidget> ViewportContainerClass;

	TSharedPtr<BaseProjectFunctionType> ProjectFuncPtr;

	FTraceSettingInfo(): CurTraceModule(ETraceModule::DisplayedUI), ViewportCharacter(nullptr), ViewportCamera(nullptr), ProjectFuncPtr(nullptr)
	{
	};
};

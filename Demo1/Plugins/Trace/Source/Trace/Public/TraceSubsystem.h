// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"

#include "TraceDefinition.h"
#include "SceneUIActor.h"
// #include "TraceUIViewport.h"
#include "TraceUIViewport.h"

#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"

#include "TraceSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnObjectTraceStarted, const FTraceSettingInfo&, CurTraceModule);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnObjectTraceEnded, const FTraceSettingInfo&, CurTraceModule);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnObjectTraceCompleted, const FString&, ObjectName);

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class TRACE_API UTraceSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	UTraceSubsystem();

	UFUNCTION(BlueprintCallable, Category="TraceCore")
	void TraceSetting(FTraceSettingInfo NewTraceModule, FString ProjectFunctionName = "VerticalRectangle");

	UFUNCTION(BlueprintCallable, Category="TraceCore")
	void Start();

	UFUNCTION(BlueprintCallable, Category="TraceCore")
	void End();

	void Update(float DeltaTime);
	void Clear();


	TWeakObjectPtr<ASceneUIActor>* FindOrCreateUIActor(const TTuple<FString, TWeakObjectPtr<AActor>>& PairIt, const FVector& ObjectLocation);
	bool CreateUIToViewport(const TTuple<FString, TWeakObjectPtr<AActor>>& It);
	void MoveUIWidget(const TTuple<FString, TWeakObjectPtr<AActor>>& It);
	void ToggleViewport(bool NewAnchoring);

	/**
	 * 获取边界函数XY坐标的最大值
	 * @return XY坐标的最大值
	 */
	UFUNCTION(BlueprintCallable)
	FVector2D GetProjectCoordinateLimit() const;

	UFUNCTION()
	void GetSelectedUIStyle(const FString TraceActorName, const FVector2D Parameter2D,
	                        EUIStyleType& StyleType, FVector4& Data) const;

	// UFUNCTION(BlueprintCallable,BlueprintNativeEvent)
	// void a();

	UFUNCTION(BlueprintCallable)
	bool UpdateLimitFuncRange(FVector2D NewRange);

	/* -------------------- Function -------------------- */

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="TraceCore|Function")
	bool CharacterIsMoveOrRotate() const
	{
		if (const ACharacter* CurCharacter = TraceModule.ViewportCharacter.Get())
		{
			return !(CurCharacter->GetCapsuleComponent()->GetPhysicsLinearVelocity().Equals(FVector(0, 0, 0), 0.001)) ||
				!(CurCharacter->GetCapsuleComponent()->GetPhysicsAngularVelocityInDegrees().Equals(FVector(0, 0, 0), 0.001));
		}
		return false;
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="TraceCore|Function")
	FVector2D GetWorldLocationToScreenPosition(APlayerController* PlayerController, FVector WorldLocation, bool bPlayerViewportRelative,
	                                           bool& bProject) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="TraceCore|Function")
	bool CheckWorldLocationIsExistViewport(FVector ObjectLocation, FVector CameraLocation, const float FOVSize, FVector CameraForward,
	                                       float& ArcoDegrees) const;

	UFUNCTION(BlueprintCallable)
	bool CheckCoordinateIsExistRange(FVector2D Target, FVector2D Range, bool bIsEqual = false);

	UFUNCTION(BlueprintCallable)
	FVector2D ClampCoordinate(FVector2D Target, FVector2D Range) const;

	UFUNCTION(BlueprintCallable)
	FVector2D GetClosestLimitCoordinate(FVector2D Target, FVector2D Range) const;

	/**
	 * 获取WorldSpace转到CameraSpace的坐标
	 * @param PlayerController 和Camera对应的Controller
	 * @param WorldLocation Actor的世界坐标
	 * @param Result 映射到屏幕上的二维坐标
	 */
	UFUNCTION(BlueprintCallable)
	void GetProjectToScreen(APlayerController* PlayerController, FVector WorldLocation, FVector2D& Result);

	UFUNCTION(BlueprintCallable)
	FVector2D GetFullScreenSize() const;

	/* -------------------- Function -------------------- */


	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;


	/* Delegate */

	UPROPERTY(BlueprintAssignable)
	FOnObjectTraceStarted OnObjectTraceStarted;

	UPROPERTY(BlueprintAssignable)
	FOnObjectTraceEnded OnObjectTraceEnded;

	UPROPERTY(BlueprintAssignable)
	FOnObjectTraceCompleted OnObjectTraceCompleted;

	/* Delegate */

	UPROPERTY()
	FTraceSettingInfo TraceModule;

	UPROPERTY(EditAnywhere, Category="TraceSubsystemData")
	float ProjectViewportScale = 0.8f;

	UPROPERTY(BlueprintReadOnly, Category="TraceSubsystemData")
	bool bIsTracing = false;

	UPROPERTY(BlueprintReadOnly, Category="TraceSubsystemData")
	bool bIsNeedfulSetting = false;

	TWeakObjectPtr<UTraceUIViewport> ViewportWidgetWeakPtr;

private:
	bool TraceInit();

	TMap<FString, TWeakObjectPtr<AActor>> TraceObjectMap;
	TMap<TWeakObjectPtr<AActor>, TWeakObjectPtr<ASceneUIActor>> TraceUIActorMap;
	/**
	 * 存储追踪的Actor对应的常驻UI和边界UI，0-NormalWidget，1-LimitWidget
	 */
	TMap<TWeakObjectPtr<AActor>, TArray<TWeakObjectPtr<UUserWidget>>> TraceUIWidgetMap;
};

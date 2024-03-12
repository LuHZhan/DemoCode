// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"

#include "TraceDefinition.h"
#include "SceneUIActor.h"

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
UCLASS(Blueprintable)
class TRACE_API UTraceSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="TraceCore")
	void TraceSetting(FTraceSettingInfo NewTraceModule);

	UFUNCTION(BlueprintCallable, Category="TraceCore")
	void Start();

	UFUNCTION(BlueprintCallable, Category="TraceCore")
	void End();

	void Update(float DeltaTime);
	void Clear();

	TWeakObjectPtr<ASceneUIActor>* FindOrCreateUIActor(const TTuple<FString, TWeakObjectPtr<AActor>>& PairIt, const FVector& ObjectLocation);
	void CreateUIToViewport(const TTuple<FString, TWeakObjectPtr<AActor>>& It);
	void MoveUIWidget(const TTuple<FString, TWeakObjectPtr<AActor>>& It);

	/**/

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
	FVector2D GetWorldLocationToScreenPosition(APlayerController* PlayerController, FVector WorldLocation, bool bPlayerViewportRelative) const
	{
		FVector2D ViewportPosition;
		UWidgetLayoutLibrary::ProjectWorldLocationToWidgetPosition(PlayerController, WorldLocation, ViewportPosition, bPlayerViewportRelative);
		return ViewportPosition;
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="TraceCore|Function")
	bool WorldLocationIsExistViewport(FVector ObjectLocation, FVector CameraLocation, const float FOVSize, FVector CameraForward, float& ArcoDegrees) const;

	/**/

	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;

	/**/

	UPROPERTY()
	FTraceSettingInfo TraceModule;

	UPROPERTY(BlueprintAssignable)
	FOnObjectTraceStarted OnObjectTraceStarted;

	UPROPERTY(BlueprintAssignable)
	FOnObjectTraceEnded OnObjectTraceEnded;

	UPROPERTY(BlueprintAssignable)
	FOnObjectTraceCompleted OnObjectTraceCompleted;

	UPROPERTY(BlueprintReadOnly, Category="TraceData")
	bool bIsTracing;

private:
	bool TraceInit();

	TMap<FString, TWeakObjectPtr<AActor>> TraceObjectMap;
	TMap<TWeakObjectPtr<AActor>, TWeakObjectPtr<ASceneUIActor>> TraceUIActorMap;
	TMap<TWeakObjectPtr<AActor>, TWeakObjectPtr<UUserWidget>> TraceUIWidgetMap;
};

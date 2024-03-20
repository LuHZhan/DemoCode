// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/Actor.h"
#include "SceneUIActor.generated.h"

UCLASS()
class TRACE_API ASceneUIActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASceneUIActor();

	UFUNCTION(BlueprintCallable)
	virtual void SetVisible(bool NewVisible, bool bForce = false);

	UFUNCTION(BlueprintCallable)
	virtual void SetSelected(bool bSelected, bool bDispatchSelectedEvent = true);

	UFUNCTION(BlueprintNativeEvent)
	void OnSetVisible(bool NewVisible);

	UFUNCTION(BlueprintImplementableEvent)
	void OnSelected(bool bSelected);

	UFUNCTION(BlueprintCallable)
	void UpdateUIData()
	{
		UI->SetWidgetSpace(EWidgetSpace::Screen);
		UI->SetWidgetClass(UIClass);
	}

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="UI")
	UWidgetComponent* UI;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="UI")
	TSubclassOf<UUserWidget> UIClass;

	UPROPERTY(EditAnywhere, Category="TraceUIData")
	bool bVisible;

	UPROPERTY(EditAnywhere, Category="TraceUIData")
	bool bIsSelected;

	UPROPERTY(EditAnywhere, Category="TraceUIData")
	FString UIName;
};

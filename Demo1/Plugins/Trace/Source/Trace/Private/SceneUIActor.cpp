// Fill out your copyright notice in the Description page of Project Settings.


#include "SceneUIActor.h"

// Sets default values
ASceneUIActor::ASceneUIActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	UI = CreateDefaultSubobject<UWidgetComponent>(TEXT("UI"));
	bVisible = true;
	bIsSelected = true;
}

void ASceneUIActor::SetVisible(bool NewVisible, bool bForce)
{
	if (bForce)
	{
		bVisible = NewVisible;
		OnSetVisible(bVisible);
	}
	else
	{
		if (NewVisible != bVisible)
		{
			bVisible = NewVisible;
			OnSetVisible(bVisible);
		}
	}
}

void ASceneUIActor::SetSelected(bool bSelected, bool bDispatchSelectedEvent)
{
	if (bIsSelected != bSelected)
	{
		bIsSelected = bSelected;
		OnSelected(bSelected);
	}
}

void ASceneUIActor::OnSetVisible_Implementation(bool NewVisible)
{
	UI->SetVisibility(NewVisible);
}

// Called when the game starts or when spawned
void ASceneUIActor::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ASceneUIActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

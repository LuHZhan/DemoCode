// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MathDefinition.generated.h"

USTRUCT(Blueprintable, BlueprintType)
struct FIntVector2D
{
	GENERATED_BODY()
	FIntVector2D()
	{
		X = 0;
		Y = 0;
	}

	FIntVector2D(int _X, int _Y): X(_X), Y(_Y)
	{
	};


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int X;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Y;
};

// Fill out your copyright notice in the Description page of Project Settings.


#include "BackpackFunLibrary.h"

TArray<FItemsArray> UBackpackFunLibrary::FiltrationFromTotalItemsInfo(const TArray<FItemsInfo>& TotalItemsInfo, const FBackpackSettingInfo& SettingInfo)
{
	TArray<FItemsArray> Result;
	int ForCount = TotalItemsInfo.Num() / SettingInfo.MaxSize.X;
	// int Remainder = TotalItemsInfo.Num() % SettingInfo.MaxSize.X;
	TArray<FItemsInfo> ItemsArray;

	int CurIndex = 1;
	for (const FItemsInfo& Element : TotalItemsInfo)
	{
		ItemsArray.Add(Element);
		if (ForCount == 0)
		{
			continue;
		}
		if (CurIndex == SettingInfo.MaxSize.X)
		{
			Result.Add(FItemsArray(ItemsArray));
			ItemsArray.Empty();
			CurIndex = 0;
			ForCount--;
		}
		CurIndex++;
	}

	Result.Add(FItemsArray(ItemsArray));
	ItemsArray.Empty();

	return Result;
}

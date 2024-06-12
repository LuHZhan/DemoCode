// Fill out your copyright notice in the Description page of Project Settings.


#include "DevDiarySubsystem.h"


void UDevDiarySubsystem::Generic_SortWithFindFunction(void* TargetArray, FArrayProperty* ArrayProp, UObject* OwnerObject, UFunction* SortRuleFunc)
{
	if (!SortRuleFunc || !OwnerObject || !TargetArray)
	{
		return;
	}

	const FBoolProperty* ReturnParam = static_cast<const FBoolProperty*>(SortRuleFunc->GetReturnProperty());
	if (ReturnParam == nullptr)
	{
		return;
	}

	FScriptArrayHelper ArrayHelper(ArrayProp, TargetArray);
	const FProperty* InnerProp = ArrayProp->Inner;

	const int32 Len = ArrayHelper.Num();
	const int32* LenPtr = &Len;
	const int32 PropertySize = InnerProp->ElementSize * InnerProp->ArrayDim;

	uint8* Parameters = (uint8*)FMemory::Malloc(PropertySize * 2 + 1);
	for (int32 i = 0; i < Len; i++)
	{
		for (int32 j = 0; j < Len - i - 1; j++)
		{
			FMemory::Memzero(Parameters, PropertySize * 2 + 1);
			InnerProp->CopyCompleteValueFromScriptVM(Parameters, ArrayHelper.GetRawPtr(j));
			InnerProp->CopyCompleteValueFromScriptVM(Parameters + PropertySize, ArrayHelper.GetRawPtr(j + 1));
			OwnerObject->ProcessEvent(SortRuleFunc, Parameters);
			if (ReturnParam && ReturnParam->GetPropertyValue(Parameters + PropertySize * 2))
			{
				ArrayHelper.SwapValues(j, j + 1);
			}
		}
	}
	FMemory::Free(Parameters);
}

void UDevDiarySubsystem::ExecuteDelegate()
{
	TArray<int> Array;
	if (OnUnicastRetValParam.IsBound())
	{
		FString Input = "OnUnicastRetValParam";
		OnUnicastRetValParam.Execute(Input);
	}

	if (OnMulticastParam.IsBound())
	{
		OnMulticastParam.Broadcast(10);
	}
}

FString UDevDiarySubsystem::OnUnicastFunc(FString& Input)
{
	return Input;
}

FString UDevDiarySubsystem::UnicastUFUNC(FString& Input)
{
	return Input;
}

FString UDevDiarySubsystem::MemberUnicastFunc(FString& Input)
{
	return Input;
}

bool UDevDiarySubsystem::DynamicUnicastFunc(int Number)
{
	return false;
}

void UDevDiarySubsystem::DynamicMulticastFunc(int Number)
{
}

void UDevDiarySubsystem::BindDelegate()
{
	// --------静态单播--------
	// 注意拥有返回值的Delegate无法使用ExecuteIfBound，需要手动调用IsBound来检测有效性，不然有可能会触发内存违规操作
	OnUnicastRetValParam.BindStatic(&UDevDiarySubsystem::OnUnicastFunc);

	// BindRaw函数用于绑定普通c++对象的成员函数，若该c++对象已被销毁，触发代理执行该对象的成员函数，将会导致内存违规操作
	// 由于绑定的是普通C++对象，UE反射系统无法获知这个C++对象的状态，这个违规操作就无法通过 IsBound 或者 ExecuteIfBound 来检测出来，只有通过if(object)来阻止
	OnUnicastRetValParam.BindRaw(&this->Cpp1, &PureCpp::PrintPureCpp);

	// 若lambda表达式捕获外部变量已被销毁，触发代理执行lambda表达式，将会导致内存违规操作。和BindRaw同理无法通过 IsBound 或者 ExecuteIfBound 来避免
	OnUnicastRetValParam.BindLambda([this](FString& Input)-> FString
	{
		return this->Cpp1.PrintPureCpp(Input);
	});

	// 绑定Lambda函数(可捕获参数)，但是会将其与一个UObject对象进行弱引用关联(不影响该对象被gc回收)
	// 若这个UObject对象被gc回收，执行调用IsBound或ExecuteIfBound会检测到该UObject无效，则可避免内存违规操作
	UObject* Object = NewObject<UObject>(this, UObject::StaticClass());
	OnUnicastRetValParam.BindWeakLambda(Object, [this](FString& Input)-> FString
	{
		return this->Cpp1.PrintPureCpp(Input);
	});

	// 绑定UObject的成员函数，执行调用IsBound或ExecuteIfBound会检测该UObject的有效性，避免内存违规操作,这个成员函数加不加UFUNCTION()都可以绑定
	OnUnicastRetValParam.BindUObject(this, &UDevDiarySubsystem::MemberUnicastFunc);

	// 绑定UObject的UFUNCTION()成员函数，执行调用IsBound或ExecuteIfBound会检测该UObject的有效性，避免内存违规操作
	// BindUFunction需要用到UE4的反射机制，因此回调函数需要用UFUNCTION()宏包住，否则UE无法通过FunctionName查找到对应的函数，会导致绑定失败
	OnUnicastRetValParam.BindUFunction(this, FName("UnicastUFUNC"));
	OnUnicastRetValParam.BindUFunction(this, STATIC_FUNCTION_FNAME(TEXT("ADelegateTest::PrintTestUFunction")));

	// 绑定被UE共享引用所引用的普通C++对象的成员函数，解决BindRow可能触发的内存违规操作, BindThreadSafeSP与BindSP相比，只是使用了线程安全的共享引用
	const TSharedRef<PureCpp> CppRef = MakeShareable(new PureCpp());
	OnUnicastRetValParam.BindSP(CppRef, &PureCpp::PrintPureCpp);

	// 还存在一系列CreateThreadSafeSP、CreateStatic等，它们的使用方法和BindXXX大致相同。区别在于CreateXXX是static函数，且返回了一个Delegate，而不是直接赋值给 *this
	auto Var = OnUnicastRetValParam.CreateSP(CppRef, &PureCpp::PrintPureCpp);

	// --------静态多播--------
	// FDelegate就是一个参数类型和这个多播相同的一个单播，从而推测出多播就是保存了多个单播，进而实现了可以绑定多个函数指针,查看AddXXX源码发现，就是通过Add来实现的
	FMulticastParam::FDelegate TempDelegate = FMulticastParam::FDelegate::CreateLambda([](int32 Number)
	{
		UE_LOG(LogTemp, Warning, TEXT("MulticastParam::FDelegate::CreateLambda %d"), Number);
	});
	FDelegateHandle Handle = OnMulticastParam.Add(TempDelegate); // 句柄用于识别绑定在多播上的对象或者函数，可以通过它来解除绑定

	// --------动态单播--------
	// BindDynamic是UE给我们定义好的宏，就是根据函数指针解析函数名字
	OnDynamicUnicastParam.BindDynamic(this, &UDevDiarySubsystem::DynamicUnicastFunc);

	// --------动态多播--------
	// 编译时不会对FScriptDelegate绑定的函数指针类型进行检查，也就是说如果FScriptDelegate对象绑定的函数指针参数与动态多播类型不一致是不会报错的
	// 经测试动态多播会把参数依次填充到绑定的函数上面，如果出现参数类型不一致Editor就会崩溃，若一致不会崩溃，但若出现空指针引用依然会导致Editor崩溃，若绑定的函数有返回值不会导致Editor崩溃。
	// 重复绑定UFUNCTION函数会导致运行时错误,综上，不推荐使用Add对动态多播进行绑定，若FunctionName输入错误，会导致不确定因素发生
	TScriptDelegate<> TempDyMulDelegate;
	TempDyMulDelegate.BindUFunction(this, "DynamicMulticastFunc");
	OnDynamicMulticastParam.Add(TempDyMulDelegate);

	// 根据FScriptDelegate对象添加绑定，但会检测是否重复
	// 当FScriptDelegate对象绑定的函数指针参数与动态多播类型不一致时，Broadcast会使编辑器直接崩溃。若FScriptDelegate对象绑定的函数指针有返回值，不会使编辑器崩溃
	OnDynamicMulticastParam.AddUnique(TempDyMulDelegate);

	// 拥有两个重载:根据FScriptDelegate对象或者FunctionName解除绑定
	OnDynamicMulticastParam.Remove(TempDyMulDelegate);
	OnDynamicMulticastParam.Remove(this, FName("DynamicMulticastFunc"));
}

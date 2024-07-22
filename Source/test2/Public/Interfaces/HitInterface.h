// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "HitInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UHitInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class TEST2_API IHitInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	// �����Լ��� �ƴ����� �������̵��� �� ����
	// �𸮾� ������ �츮�� �������̵��� �� �ִ� �����Լ��� ����������, �� �Լ� ��ü�� �������Ʈ���� ������ �����ϵ��� �Ѵ�
	// �𸮾󿡼� ������ִ� C++ ���� �����Լ��� ������ �Լ� �̸� �ڿ� _Implementation�� �پ�� �Ѵ�.
	UFUNCTION(BlueprintNativeEvent)
	void GetHit(const FVector& ImpactPoint, AActor* Hitter);
};

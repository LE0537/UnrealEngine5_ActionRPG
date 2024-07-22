// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BossHUD.generated.h"

class UProgressBar;
class UTextBlock;

UCLASS()
class TEST2_API UBossHUD : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetHealthPercent(float Percent);
	void SetName(FString Name);


	UPROPERTY(meta = (BindWidget))
	UProgressBar* HealthBar;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* BossName;
};

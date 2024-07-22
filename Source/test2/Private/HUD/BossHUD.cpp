// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/BossHUD.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UBossHUD::SetHealthPercent(float Percent)
{
	if (HealthBar)
	{
		HealthBar->SetPercent(Percent);
	}
}

void UBossHUD::SetName(FString Name)
{
	if (BossName)
	{
		const FText Text = FText::FromString(Name);
		BossName->SetText(Text);
	}
}

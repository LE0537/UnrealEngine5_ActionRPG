// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "QuestNotification.generated.h"

/**
 * 
 */
class UTextBlock;

UCLASS()
class TEST2_API UQuestNotification : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void QuestStartNotification(const FString& Text);
	void QuestCompleteNotification();
	void FadeInNotification();
	void HoldNotification();
	void FadeOutNotification();
private:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* QuestNotification;

	FTimerHandle TextboxTimer;
	float FadeTime = 1.f;
	float CurrentTime = 0.f;
	float HoldTime = 2.f;

	TArray<FString> NotificationQueue;
};

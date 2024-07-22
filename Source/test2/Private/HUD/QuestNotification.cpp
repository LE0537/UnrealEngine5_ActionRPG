// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/QuestNotification.h"
#include "Components/TextBlock.h"

void UQuestNotification::QuestStartNotification(const FString& Text)
{
	//FText InputText = FText::FromString(Text);

	if (NotificationQueue.Num() == 0)
	{
		if (IsValid(QuestNotification))
		{
			QuestNotification->SetText(FText::FromString(Text));
		}

		NotificationQueue.Add(Text);

		AddToViewport();

		FadeInNotification();
	}

	else if (NotificationQueue.Num() > 0)
	{
		NotificationQueue.Add(Text);
	}
}

void UQuestNotification::QuestCompleteNotification()
{
	FString String = FString(TEXT("Quest Clear!"));
	FText Text = FText::FromString(String);

	if (NotificationQueue.Num() == 0)
	{
		if (IsValid(QuestNotification))
		{
			QuestNotification->SetText(Text);
		}

		NotificationQueue.Add(String);

		AddToViewport();

		FadeInNotification();
	}

	else if (NotificationQueue.Num() > 0)
	{
		NotificationQueue.Add(String);
	}
}

void UQuestNotification::FadeInNotification()
{
	UWorld* World = GetWorld();

	if (World)
	{
		float DeltaTime = World->GetDeltaSeconds() == 0.f ? 0.016f : World->GetDeltaSeconds();
		World->GetTimerManager().SetTimer(TextboxTimer, this, &UQuestNotification::FadeInNotification, DeltaTime);
		CurrentTime += DeltaTime;
		QuestNotification->SetRenderOpacity(CurrentTime / FadeTime);

		if (CurrentTime >= FadeTime)
		{
			World->GetTimerManager().ClearTimer(TextboxTimer);
			CurrentTime = 0.f;
			HoldNotification();
			return;
		}
	}
}

void UQuestNotification::HoldNotification()
{
	UWorld* World = GetWorld();

	if (World)
	{
		float DeltaTime = World->GetDeltaSeconds();
		World->GetTimerManager().SetTimer(TextboxTimer, this, &UQuestNotification::HoldNotification, DeltaTime);
		CurrentTime += DeltaTime;

		if (CurrentTime >= HoldTime)
		{
			World->GetTimerManager().ClearTimer(TextboxTimer);
			CurrentTime = 0.f;
			FadeOutNotification();
			return;
		}
	}
}

void UQuestNotification::FadeOutNotification()
{
	UWorld* World = GetWorld();

	if (World)
	{
		float DeltaTime = World->GetDeltaSeconds();
		World->GetTimerManager().SetTimer(TextboxTimer, this, &UQuestNotification::FadeOutNotification, DeltaTime);
		CurrentTime += DeltaTime;
		QuestNotification->SetRenderOpacity((FadeTime - CurrentTime) / FadeTime);

		if (CurrentTime >= FadeTime)
		{
			World->GetTimerManager().ClearTimer(TextboxTimer);
			CurrentTime = 0.f;
			NotificationQueue.RemoveAt(0);

			if (NotificationQueue.Num() == 0)
			{
				RemoveFromViewport();
				return;
			}

			else if (NotificationQueue.Num() > 0)
			{
				QuestNotification->SetText(FText::FromString(NotificationQueue[0]));
				FadeInNotification();
				return;
			}
			
		}
	}
}


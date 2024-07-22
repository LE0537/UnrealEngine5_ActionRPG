// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/SlashOverlay.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "HUD/QuestNotification.h"
#include "HUD/QuestText.h"
#include "HUD/QuestUI.h"
#include "Components/VerticalBox.h"

void USlashOverlay::SetHealthBarPercent(float Percent)
{
	if (HealthProgressBar)
	{
		HealthProgressBar->SetPercent(Percent);
	}
}

void USlashOverlay::SetManaBarPercent(float Percent)
{
	if (ManaProgressBar)
	{
		ManaProgressBar->SetPercent(Percent);
	}
}

void USlashOverlay::SetGold(int32 Gold)
{
	if (GoldText)
	{
		const FString String = FString::Printf(TEXT("%d"), Gold);
		const FText Text = FText::FromString(String);
		GoldText->SetText(Text);
	}
}

void USlashOverlay::SetSouls(int32 Souls)
{
	if (SoulsText)
	{
		const FString String = FString::Printf(TEXT("%d"), Souls);
		const FText Text = FText::FromString(String);
		SoulsText->SetText(Text);
	}
}

void USlashOverlay::SetHPPotion(int32 Potion)
{
	if (HPPotionText)
	{
		const FString String = FString::Printf(TEXT("%d"), Potion);
		const FText Text = FText::FromString(String);
		HPPotionText->SetText(Text);
	}
}

void USlashOverlay::SetMPPotion(int32 Potion)
{
	if (MPPotionText)
	{
		const FString String = FString::Printf(TEXT("%d"), Potion);
		const FText Text = FText::FromString(String);
		MPPotionText->SetText(Text);
	}
}

void USlashOverlay::SetSkillOneBarPercent(float Percent)
{
	SkillOneProgressBar->SetPercent(Percent);
}

void USlashOverlay::SetSkillThreeBarPercent(float Percent)
{
	SkillThreeProgressBar->SetPercent(Percent);
}

void USlashOverlay::SetSkillFourBarPercent(float Percent)
{
	SkillFourProgressBar->SetPercent(Percent);
}

void USlashOverlay::SetSkillOneCooldownCounter(float Percent)
{
	FString String = (Percent > 0.f) ? FString::Printf(TEXT("%.1f"), Percent) : FString();
	FText Text = FText::FromString(String);
	SkillOneCooldownCounter->SetText(Text);
}

void USlashOverlay::SetSkillThreeCooldownCounter(float Percent)
{
	FString String = (Percent > 0.f) ? FString::Printf(TEXT("%.1f"), Percent) : FString();
	FText Text = FText::FromString(String);
	SkillThreeCooldownCounter->SetText(Text);
}

void USlashOverlay::SetSkillFourCooldownCounter(float Percent)
{
	FString String = (Percent > 0.f) ? FString::Printf(TEXT("%.1f"), Percent) : FString();
	FText Text = FText::FromString(String);
	SkillFourCooldownCounter->SetText(Text);
}

void USlashOverlay::SetSkillThreeActiveIcon(bool Activation)
{
	Activation ? SkillThreeActiveIcon->SetVisibility(ESlateVisibility::Visible) : SkillThreeActiveIcon->SetVisibility(ESlateVisibility::Hidden);
}

void USlashOverlay::SetRollBarPercent(float Percent)
{
	RollProgressBar->SetPercent(Percent);
}

void USlashOverlay::SetDodgeBarPercent(float Percent)
{
	DodgeProgressBar->SetPercent(Percent);
}

void USlashOverlay::SetRollCooldownCounter(float Cooldown)
{
	FString String = (Cooldown > 0.f) ? FString::Printf(TEXT("%.1f"), Cooldown) : FString::Printf(TEXT(""));
	FText Text = FText::FromString(String);
	RollCooldownCounter->SetText(Text);
}

void USlashOverlay::SetDodgeCooldownCounter(float Cooldown)
{
	FString String = (Cooldown > 0.f) ? FString::Printf(TEXT("%.1f"), Cooldown) : FString::Printf(TEXT(""));
	FText Text = FText::FromString(String);
	DodgeCooldownCounter->SetText(Text);
}

void USlashOverlay::SetDodgeChargeCounter(int32 Count)
{
	const FString String = FString::Printf(TEXT("%d"), Count);
	const FText Text = FText::FromString(String);
	DodgeChargeCounter->SetText(Text);
}

void USlashOverlay::SetSkillThreeActiveCounter(float Count)
{
	FString String = (Count > 0.f) ? FString::Printf(TEXT("%.1f"), Count) : FString::Printf(TEXT(""));
	FText Text = FText::FromString(String);
	SkillThreeActiveCounter->SetText(Text);
}

void USlashOverlay::SetSkillOneChargeCounter(int32 Count)
{
	const FString String = FString::Printf(TEXT("%d"), Count);
	const FText Text = FText::FromString(String);
	SkillOneChargeCounter->SetText(Text);
}

void USlashOverlay::SetHPCounter(float CurrentHP, float MaxHP)
{
	FString String = FString::Printf(TEXT("%d / %d"), static_cast<int32>(CurrentHP), static_cast<int32>(MaxHP));
	HPCounter->SetText(FText::FromString(String));
}

void USlashOverlay::SetMPCounter(float CurrentMP, float MaxMP)
{
	FString String = FString::Printf(TEXT("%d / %d"), static_cast<int32>(CurrentMP), static_cast<int32>(MaxMP));
	MPCounter->SetText(FText::FromString(String));
}

void USlashOverlay::AddQuest(const FString& QuestName, const FString& QuestEntryString, int32 QuestObjective, int32 QuestCurrentProgress, bool bIsDisplayingNumbers)
{
	FString String = TEXT("QUEST");
	FText Text = FText::FromString(String);

	if (QuestVector.Num() == 0)
	{
		QuestHeader->SetText(Text);
	}

	QuestVector.Add(FQuest(QuestName, QuestEntryString, QuestObjective, QuestCurrentProgress, bIsDisplayingNumbers));

	if (QuestUI && QuestTextClass)
	{
		UQuestText* NewQuestText = CreateWidget<UQuestText>(GetWorld(), QuestTextClass);
		if (NewQuestText)
		{
			NewQuestText->SetQuestName(QuestName);

			if (bIsDisplayingNumbers)
			{
				NewQuestText->SetQuestText(FText::FromString(QuestEntryString +
					FString(TEXT(" ")) +
					FString::Printf(TEXT("%d"), QuestCurrentProgress) +
					FString(TEXT("/")) +
					FString::Printf(TEXT("%d"), QuestObjective)
				));
			}

			else
			{
				NewQuestText->SetQuestText(FText::FromString(QuestEntryString));
			}

			QuestUI->QuestList->AddChild(NewQuestText);
		}
	}

	
	QuestNotification->QuestStartNotification(QuestName);
}

int32 USlashOverlay::GetCurrentQuestProgress(const FString& QuestName)
{
	int32 Selected = QuestNameToIndex(QuestName);

	if (Selected == -1)
	{
		UE_LOG(LogTemp, Warning, TEXT("Quest name %s doesn't exist."), *QuestName);
		return -1;
	}

	return QuestVector[Selected].QuestCurrentProgress;
}

bool USlashOverlay::UpdateQuest(const FString& QuestName, int32 QuestCurrentProgress)
{
	int32 Selected = QuestNameToIndex(QuestName);

	if (Selected == -1)
	{
		UE_LOG(LogTemp, Warning, TEXT("Quest name %s doesn't exist."), *QuestName);
		return false;
	}

	QuestVector[Selected].QuestCurrentProgress = QuestCurrentProgress;
	QuestUI->UpdateQuest(QuestName, QuestVector[Selected].QuestEntry, QuestVector[Selected].QuestObjective, QuestCurrentProgress, QuestVector[Selected].IsDisplayingNumbers);

	if (QuestVector[Selected].QuestCurrentProgress >= QuestVector[Selected].QuestObjective)
	{
		CompleteQuest(Selected);
		return true;
	}

	return false;
}

void USlashOverlay::CompleteQuest(int32 Index)
{
	FQuest CompletedQuest = QuestVector[Index];
	QuestVector.RemoveAt(Index);

	CompletedQuestVector.Add(CompletedQuest);

	// 퀘스트 완료 시 UI 업데이트
	if (QuestUI)
	{
		QuestUI->RemoveQuest(CompletedQuest.QuestName);
	}

	QuestNotification->QuestCompleteNotification();
}

bool USlashOverlay::IsQuestActive(const FString& QuestName)
{
	for (int32 i = 0; i < QuestVector.Num(); ++i)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s, %s"),*QuestVector[i].QuestName, *QuestName)
		if (QuestVector[i].QuestName == QuestName)
		{
			return true;
		}
	}
	return false;
}

void USlashOverlay::PopCompletedQuest()
{
	CompletedQuestVector.Pop();
}

void USlashOverlay::ShowQuestUI()
{
	if (QuestUI)
	{
		QuestUI->AddToViewport();
	}
}

void USlashOverlay::HideQuestUI()
{
	if (QuestUI)
	{
		QuestUI->RemoveFromViewport();
	}
}

void USlashOverlay::NativeConstruct()
{
	Super::NativeConstruct();

	// QuestUI 위젯 생성 및 화면에 추가
	FString BlueprintPath = TEXT("/Game/Blueprints/HUD/WBP_QuestUI.WBP_QuestUI_C");
	UClass* BlueprintClass = Cast<UClass>(StaticLoadObject(UClass::StaticClass(), nullptr, *FName(*BlueprintPath).ToString()));

	QuestUI = CreateWidget<UQuestUI>(GetWorld(), BlueprintClass);

	BlueprintPath = TEXT("/Game/Blueprints/HUD/WBP_QuestNotification.WBP_QuestNotification_C");
	QuestNotificationClass = Cast<UClass>(StaticLoadObject(UClass::StaticClass(), nullptr, *FName(*BlueprintPath).ToString()));

	QuestNotification = CreateWidget<UQuestNotification>(GetWorld(), QuestNotificationClass);
}

int32 USlashOverlay::QuestNameToIndex(const FString& QuestName)
{
	int32 ret = -1;

	for (int32 i = 0; i < QuestVector.Num(); ++i)
	{
		if (QuestVector[i].QuestName == QuestName)
		{
			ret = i;
			break;
		}
	}

	return ret;
}

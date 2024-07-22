// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/QuestUI.h"
#include "Components/VerticalBox.h"
#include "HUD/QuestText.h"

void UQuestUI::RemoveQuest(FString QuestName)
{
    for (int32 i = 0; i < QuestList->GetChildrenCount(); i++)
    {
        UQuestText* QuestItem = Cast<UQuestText>(QuestList->GetChildAt(i));
        if (QuestItem && QuestItem->GetQuestName() == QuestName)
        {
            QuestList->RemoveChildAt(i);
            break;
        }
    }
}

void UQuestUI::UpdateQuest(const FString& QuestName, const FString& QuestEntry, int32 QuestObjective, int32 QuestCurrentProgress, bool bIsDisplayingNumbers)
{
    for (int32 i = 0; i < QuestList->GetChildrenCount(); i++)
    {
        UQuestText* QuestItem = Cast<UQuestText>(QuestList->GetChildAt(i));
        if (QuestItem && QuestItem->GetQuestName() == QuestName)
        {
            if (bIsDisplayingNumbers)
            {
                QuestItem->SetQuestText(FText::FromString(QuestEntry +
                    FString(TEXT(" ")) +
                    FString::Printf(TEXT("%d"), QuestCurrentProgress) +
                    FString(TEXT("/")) +
                    FString::Printf(TEXT("%d"), QuestObjective)
                ));
            }
            break;
        }
    }
}

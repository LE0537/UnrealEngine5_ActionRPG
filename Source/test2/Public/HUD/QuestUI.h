// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "QuestUI.generated.h"

/**
 * 
 */

class UVerticalBox;
UCLASS()
class TEST2_API UQuestUI : public UUserWidget
{
	GENERATED_BODY()

public:
    UPROPERTY(meta = (BindWidget))
    UVerticalBox* QuestList;

    void RemoveQuest(FString QuestName);
    void UpdateQuest(const FString& QuestName, const FString& QuestEntry, int32 QuestObjective, int32 QuestCurrentProgress, bool bIsDisplayingNumbers);
};

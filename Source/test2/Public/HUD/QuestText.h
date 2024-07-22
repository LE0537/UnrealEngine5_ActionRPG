// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "QuestText.generated.h"

/**
 * 
 */

class UTextBlock;

UCLASS()
class TEST2_API UQuestText : public UUserWidget
{
	GENERATED_BODY()
	
public:
    virtual void NativeConstruct() override;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* QuestText;

    FString QuestName;

    void SetQuestName(FString NewQuestName);
    FString GetQuestName() const;
    void SetQuestText(FText NewText);
    FString GetQuestText() const;
};

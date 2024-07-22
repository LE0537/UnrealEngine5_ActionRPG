// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/QuestText.h"
#include "Components/TextBlock.h"
#include "Engine/Font.h"
#include "UObject/ConstructorHelpers.h"
#include "Fonts/SlateFontInfo.h"

void UQuestText::NativeConstruct()
{
    UUserWidget::NativeConstruct();

    if (QuestText)
    {
        // 폰트 설정
        FSlateFontInfo FontInfo = QuestText->Font;
        UFont* CustomFont = Cast<UFont>(StaticLoadObject(UFont::StaticClass(), nullptr, TEXT("/Game/Fonts/Cafe24Dongdong-v2_0_Font")));
        FontInfo.FontObject = CustomFont;
        FontInfo.Size = 20;  // 폰트 크기 설정
        FontInfo.OutlineSettings.OutlineSize = 2;
        FontInfo.OutlineSettings.OutlineColor = FLinearColor::Black;
        QuestText->SetFont(FontInfo);

        // 텍스트 색깔 설정
        FSlateColor TextColor = FSlateColor(FLinearColor::White);  // 텍스트 색깔 설정
        QuestText->SetColorAndOpacity(TextColor);
    }
}

void UQuestText::SetQuestName(FString NewQuestName)
{
    QuestName = NewQuestName;
}

FString UQuestText::GetQuestName() const
{
    return QuestName;
}

void UQuestText::SetQuestText(FText NewText)
{
    if (QuestText)
    {
        QuestText->SetText(NewText);
    }
}

FString UQuestText::GetQuestText() const
{
    return QuestText ? QuestText->GetText().ToString() : FString();
}


// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SlashOverlay.generated.h"

/**
 * 
 */

class UProgressBar;
class UTextBlock;
class UImage;
class UQuestNotification;
class UMultiLineEditableTextBox;

UCLASS()
class TEST2_API USlashOverlay : public UUserWidget
{
	GENERATED_BODY()

	struct FQuest
	{
		FString QuestName;
		FString QuestEntry;
		int32	QuestObjective;
		int32	QuestCurrentProgress;
		bool	IsDisplayingNumbers;
	};

public:
	void SetHealthBarPercent(float Percent);	// ���α׷��� ���� ��ô���� �����ϴ� �Լ�
	void SetManaBarPercent(float Percent);		
	void SetGold(int32 Gold);					// ���� ���� ���� �׼��� �����ϴ� �Լ�
	void SetSouls(int32 Souls);
	void SetHPPotion(int32 Potion);
	void SetMPPotion(int32 Potion);
	void SetSkillOneBarPercent(float Percent);
	void SetSkillThreeBarPercent(float Percent);
	void SetSkillFourBarPercent(float Percent);
	void SetSkillOneCooldownCounter(float Percent);
	void SetSkillThreeCooldownCounter(float Percent);
	void SetSkillFourCooldownCounter(float Percent);
	void SetSkillThreeActiveIcon(bool Activation);
	void SetRollBarPercent(float Percent);
	void SetDodgeBarPercent(float Percent);
	void SetRollCooldownCounter(float Cooldown);
	void SetDodgeCooldownCounter(float Cooldown);
	void SetDodgeChargeCounter(int32 Count);
	void SetSkillThreeActiveCounter(float Count);
	void SetSkillOneChargeCounter(int32 Count);
	void SetHPCounter(float CurrentHP, float MaxHP);
	void SetMPCounter(float CurrentMP, float MaxMP);

	void AddQuest(const FString& QuestName, const FString& QuestEntryString, int32 QuestObjective, int32 QuestCurrentProgress, bool IsDisplayingNumbers);
	int32 GetCurrentQuestProgress(const FString& QuestName);
	bool UpdateQuest(const FString& QuestName, int32 QuestCurrentProgress);
	void CompleteQuest(int32 Index);
	bool IsQuestActive(const FString& QuestName);
	void PopCompletedQuest();
	void ShowQuestUI();
	void HideQuestUI();

	virtual void NativeConstruct() override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<class UUserWidget> QuestTextClass;

private:
	int32 QuestNameToIndex(const FString& QuestName);

	UPROPERTY(meta = (BindWidget))	// ���� �̸��� ������ �ش� ������ �����ϴ� ��ũ��
	UProgressBar* HealthProgressBar;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* ManaProgressBar;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* GoldText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* SoulsText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* HPPotionText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* MPPotionText;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* SkillOneProgressBar;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* SkillThreeProgressBar;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* SkillFourProgressBar;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* SkillOneCooldownCounter;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* SkillOneChargeCounter;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* SkillThreeCooldownCounter;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* SkillFourCooldownCounter;

	UPROPERTY(meta = (BindWidget))
	UImage* SkillThreeActiveIcon;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* DodgeProgressBar;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* DodgeCooldownCounter;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* DodgeChargeCounter;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* RollProgressBar;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* RollCooldownCounter;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* SkillThreeActiveCounter;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* QuestHeader;

	UPROPERTY(meta = (BindWidget))
	UMultiLineEditableTextBox* QuestText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* HPCounter;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* MPCounter;

	TArray<FQuest> QuestVector;

	TArray<FQuest> CompletedQuestVector;

	UPROPERTY()
	class UQuestUI* QuestUI;

	UPROPERTY(EditDefaultsOnly, Category = "Quest")
	TSubclassOf<UQuestNotification> QuestNotificationClass;

	UPROPERTY(EditDefaultsOnly, Category = "Quest")
	UQuestNotification* QuestNotification;
};

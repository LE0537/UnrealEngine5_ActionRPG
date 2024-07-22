// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BGMManager/BGMManager.h"
#include "QuestTrigger.generated.h"

class UBoxComponent;

UENUM(BlueprintType)
enum class EQuestTriggerState : uint8
{
	EQTS_QuestStarter UMETA(DisplayName = "QuestStarter"),
	EQTS_QuestFinisher UMETA(DisplayName = "QuestFinisher"),
	EQTS_QuestLink UMETA(DisplayName = "QuestLink")
};

UCLASS()

class TEST2_API AQuestTrigger : public AActor
{
	GENERATED_BODY()
	
public:	
	AQuestTrigger();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "Quest")
	FString	DesignatedQuestName;

	UPROPERTY(EditDefaultsOnly, Category = "Quest")
	FString DesignatedQuestEntry;

	UPROPERTY(EditDefaultsOnly, Category = "Quest")
	int32 DesignatedQuestObjectiveAmount;

	UPROPERTY(EditDefaultsOnly, Category = "Quest")
	FString	NextLinkQuestName;

	UPROPERTY(EditDefaultsOnly, Category = "Quest")
	FString NextLinkQuestEntry;

	UPROPERTY(EditDefaultsOnly, Category = "Quest")
	int32 NextLinkQuestObjectiveAmount;

	UPROPERTY(EditDefaultsOnly, Category = "Quest")
	bool bIsDisplayingQuestObjectiveNumber;

	UPROPERTY(EditDefaultsOnly, Category = "Quest")
	EQuestTriggerState QuestTriggerState;

	UPROPERTY(EditDefaultsOnly, Category = "BGM")
	bool bIsChangingBGM = false;

	UPROPERTY(EditDefaultsOnly, Category = "BGM")
	EBGM BGMState;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UFUNCTION()
	void OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
private:
	UPROPERTY(EditDefaultsOnly, Category = "Quest")
	UBoxComponent* TriggerBox;
};

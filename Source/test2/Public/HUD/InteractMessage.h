// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InteractMessage.generated.h"

/**
 * 
 */
class UTextBlock;
class UImage;

UCLASS()
class TEST2_API UInteractMessage : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void TurnOn();
	void TurnOff();

private:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* GrabText;

	UPROPERTY(meta = (BindWidget))
	UImage* GrabImage;
};

// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/InteractMessage.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

void UInteractMessage::TurnOn()
{
	AddToViewport();
}

void UInteractMessage::TurnOff()
{
	RemoveFromViewport();
}

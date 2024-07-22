// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Soul.generated.h"

/**
 * 
 */
class UNiagaraSystem;

UCLASS()
class TEST2_API ASoul : public AItem
{
	GENERATED_BODY()
public:
	int32 GetSoul() { return Soul; }
	void SetSoul(int32 Value) { Soul = Value; }
	
protected:
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

private:
	int32 Soul = 1;
};

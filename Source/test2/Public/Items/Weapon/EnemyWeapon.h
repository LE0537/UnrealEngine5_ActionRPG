// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Weapon/Weapon.h"
#include "EnemyWeapon.generated.h"


UCLASS()
class TEST2_API AEnemyWeapon : public AWeapon
{
	GENERATED_BODY()
private :
	virtual void BeginPlay() override;

	UFUNCTION()	//
	void OnBoxOverlapEnemy(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

};

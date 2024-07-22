// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DragonGroundSkill1.generated.h"

class UBoxComponent;

UCLASS()
class TEST2_API ADragonGroundSkill1 : public AActor
{
	GENERATED_BODY()
	
public:	
	ADragonGroundSkill1();

	void SetGroundSkill(ECollisionEnabled::Type CollisionEnabled);
	void SetSocket(USceneComponent* InParent, FName InSocketName, AActor* NewOwner, APawn* NewInstigator);
protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Collision")
	UBoxComponent* GroundSkill1Box;

private:
	UFUNCTION()
	void OnGroundSkill1BoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	void ExecuteGetHitEnemy(AActor* BoxHit);
	void AttachMeshToSocket(USceneComponent* InParent, const FName& InSocketName);
};

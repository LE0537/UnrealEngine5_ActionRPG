// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpiderEventTriger.generated.h"

class UBoxComponent;

UCLASS()
class TEST2_API ASpiderEventTriger : public AActor
{
	GENERATED_BODY()
	
public:	
	ASpiderEventTriger();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, Category = "Boss")
	TSubclassOf<class APawn> BossSpiderClass;

private:
	UPROPERTY(VisibleAnywhere, Category = "Box Properties")
	UBoxComponent* BoxTriger;

	FVector SpawnLocation;
	FRotator SpawnRotator;

	//������ ī�޶�
	UFUNCTION()
	void OnSequenceFinished();

	APlayerController* MyPlayerController;
	FTimerHandle TimerHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sequence", meta = (AllowPrivateAccess = "true"))
	class ALevelSequenceActor* SequenceActor;

	UPROPERTY()
	class ULevelSequencePlayer* SequencePlayer;

	AActor* pPlayer;

	bool isSpawned = false;
};

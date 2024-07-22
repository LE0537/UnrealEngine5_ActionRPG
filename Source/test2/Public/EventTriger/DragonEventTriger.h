// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DragonEventTriger.generated.h"

class USphereComponent;

UCLASS()
class TEST2_API ADragonEventTriger : public AActor
{
	GENERATED_BODY()
	
public:	
	ADragonEventTriger();

protected:
	virtual void BeginPlay() override;
	
	UFUNCTION()
	void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, Category = "Boss")
	TSubclassOf<class APawn> BossDragonClass;

private:
	UPROPERTY(VisibleAnywhere, Category = "Box Properties")
	USphereComponent* SphereTriger;

	FVector SpawnLocation;
	FRotator SpawnRotator;

	bool isSpawned = false;

	//시퀀스 카메라
	UFUNCTION()
	void OnSequenceFinished();

	APlayerController* MyPlayerController;
	FTimerHandle TimerHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sequence", meta = (AllowPrivateAccess = "true"))
	class ALevelSequenceActor* SequenceActor;

	UPROPERTY()
	class ULevelSequencePlayer* SequencePlayer;

	AActor* pPlayer;
};

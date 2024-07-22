// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BGMManager.generated.h"


class USoundBase;
class UAudioComponent;

UENUM(BlueprintType)
enum class EBGM : uint8
{
	EBGM_Base UMETA(DisplayName = "Base"),
	EBGM_Village UMETA(DisplayName = "Village"),
	EBGM_Dungeon UMETA(DisplayName = "Dungeon"),
	EBGM_Spider UMETA(DisplayName = "Spider"),
	EBGM_Dragon UMETA(DisplayName = "Dragon")
};

UCLASS()
class TEST2_API ABGMManager : public AActor
{
	GENERATED_BODY()
	
public:	

	// Sets default values for this actor's properties
	ABGMManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void Destroyed() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void PlayBGM(EBGM NewBGM);

	UFUNCTION(BlueprintCallable)
	void StopBGM();

	void Init();

private:
	UPROPERTY(EditAnywhere, Category = "BGM");
	TArray<USoundWave*> BGM;

	UAudioComponent* AudioComponents;
};

// Fill out your copyright notice in the Description page of Project Settings.


#include "Effect/PoisonHitEffect.h"

// Sets default values
APoisonHitEffect::APoisonHitEffect()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APoisonHitEffect::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APoisonHitEffect::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


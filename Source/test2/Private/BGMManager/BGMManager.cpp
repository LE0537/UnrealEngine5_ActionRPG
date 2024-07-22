// Fill out your copyright notice in the Description page of Project Settings.


#include "BGMManager/BGMManager.h"
#include "Sound/SoundBase.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Characters/SlashCharacter.h"

// Sets default values
ABGMManager::ABGMManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AudioComponents = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
	SetRootComponent(AudioComponents);
	AudioComponents->bAutoActivate = false;
	AudioComponents->bAutoDestroy = false;
	AudioComponents->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ABGMManager::BeginPlay()
{
	Super::BeginPlay();

	ASlashCharacter* Player = Cast<ASlashCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
	if (Player)
	{
		Player->SetBGMManager(this);
		FAttachmentTransformRules AttachmentTransformRules(EAttachmentRule::SnapToTarget, true);
		AttachToActor(Player, AttachmentTransformRules);
		AddToRoot();
	}
	
	for (int32 i = 0; i < BGM.Num(); ++i)
	{
		BGM[i]->bLooping = true;
	}
}

void ABGMManager::Destroyed()
{
	UE_LOG(LogTemp, Warning, TEXT("BGMManager Destroyed"))
}

void ABGMManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	RemoveFromRoot();
}

// Called every frame
void ABGMManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABGMManager::PlayBGM(EBGM NewBGM)
{
	int32 NewSoundIndex = static_cast<int32>(NewBGM);
	if (BGM.IsValidIndex(NewSoundIndex) && BGM[NewSoundIndex])
	{
		AudioComponents->SetSound(BGM[NewSoundIndex]);
		AudioComponents->Play();
	}
}

void ABGMManager::StopBGM()
{
	if (AudioComponents && AudioComponents->IsPlaying())
	{
		AudioComponents->Stop();
	}
}


// Fill out your copyright notice in the Description page of Project Settings.


#include "EventTriger/DragonEventTriger.h"
#include "Enemy/Boss_Dragon.h"
#include "Components/SphereComponent.h"
#include "Characters/SlashCharacter.h"
#include "LevelSequence.h"
#include "LevelSequenceActor.h"
#include "LevelSequencePlayer.h"
#include "Camera/CameraActor.h"
#include "MovieSceneSequencePlayer.h"

ADragonEventTriger::ADragonEventTriger()
{
	PrimaryActorTick.bCanEverTick = true;

	SphereTriger = CreateDefaultSubobject<USphereComponent>(TEXT("Weapon Box"));
	SphereTriger->SetupAttachment(GetRootComponent());

	SpawnLocation = { 10960.f, -16820.f, 400.01f };
	SpawnRotator = { 0.f, 100.f, 0.f };
}

void ADragonEventTriger::BeginPlay()
{
	Super::BeginPlay();
	
	SphereTriger->OnComponentBeginOverlap.AddDynamic(this, &ADragonEventTriger::OnSphereOverlap);

	if (SequenceActor)
	{
		// ���� ������ �÷��̾� ����
		SequencePlayer = ULevelSequencePlayer::CreateLevelSequencePlayer(GetWorld(), SequenceActor->GetSequence(), FMovieSceneSequencePlaybackSettings(), SequenceActor);
		if (SequencePlayer)
		{
			SequencePlayer->OnFinished.AddDynamic(this, &ADragonEventTriger::OnSequenceFinished);
		}
	}
}

void ADragonEventTriger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADragonEventTriger::OnSequenceFinished()
{
	//ĳ��Ʈ�ؼ� �÷��̾�� ����Ʈ �Լ�
	// pPlayer
	Cast<ASlashCharacter>(pPlayer)->SetCutScene(false);
	Destroy();
}


void ADragonEventTriger::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor->ActorHasTag(TEXT("EngageableTarget")))
	{
		return;
	}
	if (!isSpawned)
	{
		isSpawned = true;
		ABoss_Dragon* Boss = GetWorld()->SpawnActor<ABoss_Dragon>(BossDragonClass, SpawnLocation, SpawnRotator);
		pPlayer = OtherActor;
		Cast<ASlashCharacter>(pPlayer)->SetCutScene(true);
		SequencePlayer->Play();
	}
}

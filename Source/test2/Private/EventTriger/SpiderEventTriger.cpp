#include "EventTriger/SpiderEventTriger.h"
#include "Components/BoxComponent.h"
#include "Enemy/Boss_Wolf.h"
#include "LevelSequence.h"
#include "LevelSequenceActor.h"
#include "LevelSequencePlayer.h"
#include "Camera/CameraActor.h"
#include "MovieSceneSequencePlayer.h"
#include "Characters/SlashCharacter.h"

ASpiderEventTriger::ASpiderEventTriger()
{
	PrimaryActorTick.bCanEverTick = true;

	BoxTriger = CreateDefaultSubobject<UBoxComponent>(TEXT("Weapon Box"));
	BoxTriger->SetupAttachment(GetRootComponent());

	SpawnLocation = { -19400.f, -8700.f, 451.9f };
	SpawnRotator = { 0.f, -45.f, 0.f };
}

void ASpiderEventTriger::BeginPlay()
{
	Super::BeginPlay();

	BoxTriger->OnComponentBeginOverlap.AddDynamic(this, &ASpiderEventTriger::OnBoxOverlap);
	
	if (SequenceActor)
	{
		// ���� ������ �÷��̾� ����
		SequencePlayer = ULevelSequencePlayer::CreateLevelSequencePlayer(GetWorld(), SequenceActor->GetSequence(), FMovieSceneSequencePlaybackSettings(), SequenceActor);

		if (SequencePlayer)
		{
			SequencePlayer->OnFinished.AddDynamic(this, &ASpiderEventTriger::OnSequenceFinished);
		}
	}
}

void ASpiderEventTriger::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor->ActorHasTag(TEXT("EngageableTarget")))
	{
		UE_LOG(LogTemp, Warning, TEXT("2222"));
		return;
	}
	if (!isSpawned)
	{
		isSpawned = true;
		ABoss_Wolf* Boss = GetWorld()->SpawnActor<ABoss_Wolf>(BossSpiderClass, SpawnLocation, SpawnRotator);
		pPlayer = OtherActor;
		Cast<ASlashCharacter>(pPlayer)->SetCutScene(true);
		SequencePlayer->Play();
	}
}

void ASpiderEventTriger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASpiderEventTriger::OnSequenceFinished()
{
	UE_LOG(LogTemp, Warning, TEXT("1111"));
	//ĳ��Ʈ�ؼ� �÷��̾�� ����Ʈ �Լ�
	// pPlayer
	Cast<ASlashCharacter>(pPlayer)->SetCutScene(false);
	Destroy();
}


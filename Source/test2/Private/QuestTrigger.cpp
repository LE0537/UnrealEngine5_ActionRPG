// Fill out your copyright notice in the Description page of Project Settings.


#include "QuestTrigger.h"
#include "Components/BoxComponent.h"
#include "Characters/SlashCharacter.h"

// Sets default values
AQuestTrigger::AQuestTrigger() : QuestTriggerState(EQuestTriggerState::EQTS_QuestFinisher), DesignatedQuestObjectiveAmount(1), NextLinkQuestObjectiveAmount(1), bIsDisplayingQuestObjectiveNumber(false)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	TriggerBox->SetupAttachment(GetRootComponent());
	TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
}



// Called when the game starts or when spawned
void AQuestTrigger::BeginPlay()
{
	Super::BeginPlay();
	
	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AQuestTrigger::OnBoxOverlap);
}

// Called every frame
void AQuestTrigger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AQuestTrigger::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor->ActorHasTag(FName(TEXT("EngageableTarget"))))
	{
		ASlashCharacter* Player = Cast<ASlashCharacter>(OtherActor);

		if (bIsChangingBGM)
		{
			ABGMManager* BGMManager = Player->GetBGMManager();

			if (IsValid(BGMManager))
			{
				BGMManager->PlayBGM(BGMState);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("BGMManager is invalid!"))
			}
		}
		
		if (QuestTriggerState == EQuestTriggerState::EQTS_QuestFinisher)
		{
			int32 CurrentProgress = Player->GetQuestCurrentProgress(DesignatedQuestName);
			Player->UpdateQuest(DesignatedQuestName, CurrentProgress + 1);
		}

		else if (QuestTriggerState == EQuestTriggerState::EQTS_QuestStarter)
		{
			Player->AddQuest(DesignatedQuestName, DesignatedQuestEntry, DesignatedQuestObjectiveAmount, 0, bIsDisplayingQuestObjectiveNumber);
		}

		else if (QuestTriggerState == EQuestTriggerState::EQTS_QuestLink)
		{
			int32 CurrentProgress = Player->GetQuestCurrentProgress(DesignatedQuestName);
			Player->UpdateQuest(DesignatedQuestName, CurrentProgress + 1);
			Player->AddQuest(NextLinkQuestName, NextLinkQuestEntry, NextLinkQuestObjectiveAmount, 0, bIsDisplayingQuestObjectiveNumber);
		}
	}

	Destroy();
}


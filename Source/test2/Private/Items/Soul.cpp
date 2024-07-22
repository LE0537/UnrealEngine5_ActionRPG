// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Soul.h"
#include "Interfaces/PickupInterface.h"
#include "Characters/SlashCharacter.h"
#include "BGMManager/BGMManager.h"

void ASoul::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	IPickupInterface* PickupInterface = Cast<IPickupInterface>(OtherActor);

	if (!PickupInterface)
	{
		return;
	}

	PickupInterface->AddSouls(this);	// 플레이어 캐릭터가 충돌하면 픽업 인터페이스에 있는 아이템 포인터가 이 아이템을 가리키도록 설정

	if (OtherActor->ActorHasTag(FName(TEXT("EngageableTarget"))))
	{
		ASlashCharacter* Player = Cast<ASlashCharacter>(OtherActor);
		FString QuestNameString = FString(TEXT("마을의 언데드 사냥"));
		bool UpdateResult = false;

		if (Player->IsQuestActive(QuestNameString))
		{
			int32 QuestCurrentProgress = Player->GetQuestCurrentProgress(QuestNameString);
			UpdateResult = Player->UpdateQuest(QuestNameString, QuestCurrentProgress + 1);

			if (UpdateResult)
			{
				Player->AddQuest(FString(TEXT("언데드의 원흉 찾기")), FString(TEXT("언데드가 출몰하게 된 원흉을 찾아라.")));
				Player->GetBGMManager()->StopBGM();
				Player->GetBGMManager()->PlayBGM(EBGM::EBGM_Base);
			}
		}
	}

	SpawnPickupEffect();
	SpawnPickupSound();

	Destroy();
}

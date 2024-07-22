// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Weapon/EnemyWeapon.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

void AEnemyWeapon::BeginPlay()
{
	Init();
	WeaponBox->OnComponentBeginOverlap.AddDynamic(this, &AEnemyWeapon::OnBoxOverlapEnemy);	// ��������Ʈ �Լ��� �츮�� ���� OnBoxOverlap�� �����ϴ� ��ũ��
}

void AEnemyWeapon::OnBoxOverlapEnemy(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ActorIsSameType(OtherActor))
	{
		return;
	}

	if (OtherActor->ActorHasTag(FName(TEXT("Dead"))))
	{
		return;
	}

	if (OtherActor)
	{

		UGameplayStatics::ApplyDamage(
			OtherActor,					// ������� ���� ����, �� ������ ���� �� ������ TakeDamage �Լ��� ȣ��� ��
			Damage * BuffMultiplier,								// �������
			GetInstigator()->GetController(),	// �ν�Ƽ������(������� �� ��ü)�� �÷��̾��� ��Ʈ�ѷ�
			this,								// ������� ���������� �� ��ü�� �ڽ�(����)�� ����
			UDamageType::StaticClass()			// ����� Ÿ��
		);	// ���� Enemy.cpp���� TakeDamage�� ó���Ұ���

		ExecuteGetHitEnemy(OtherActor);

		CreateFields(OtherActor->GetActorLocation());

		WeaponBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

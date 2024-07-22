// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/DragonGroundSkill1.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Interfaces/HitInterface.h"

ADragonGroundSkill1::ADragonGroundSkill1()
{
	PrimaryActorTick.bCanEverTick = false;
	GroundSkill1Box = CreateDefaultSubobject<UBoxComponent>(TEXT("Weapon Box"));
	RootComponent = GroundSkill1Box;
	GroundSkill1Box->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ADragonGroundSkill1::SetGroundSkill(ECollisionEnabled::Type CollisionEnabled)
{
	GroundSkill1Box->SetCollisionEnabled(CollisionEnabled);
}

void ADragonGroundSkill1::SetSocket(USceneComponent* InParent, FName InSocketName, AActor* NewOwner, APawn* NewInstigator)
{
	SetOwner(NewOwner);	
	SetInstigator(NewInstigator);	
	AttachMeshToSocket(InParent, InSocketName);
}


void ADragonGroundSkill1::BeginPlay()
{
	Super::BeginPlay();
	
	GroundSkill1Box->OnComponentBeginOverlap.AddDynamic(this, &ADragonGroundSkill1::OnGroundSkill1BoxOverlap);
}

void ADragonGroundSkill1::OnGroundSkill1BoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->ActorHasTag(TEXT("BOSS")))
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
			100.f,								// �������
			GetInstigator()->GetController(),	// �ν�Ƽ������(������� �� ��ü)�� �÷��̾��� ��Ʈ�ѷ�
			this,								// ������� ���������� �� ��ü�� �ڽ�(����)�� ����
			UDamageType::StaticClass()			// ����� Ÿ��
		);	// ���� Enemy.cpp���� TakeDamage�� ó���Ұ���

		ExecuteGetHitEnemy(OtherActor);

		//GroundSkill1Box->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void ADragonGroundSkill1::ExecuteGetHitEnemy(AActor* BoxHit)
{
	IHitInterface* HitInterface = Cast<IHitInterface>(BoxHit);

	if (HitInterface)
	{
		HitInterface->Execute_GetHit(BoxHit, BoxHit->GetActorLocation(), GetOwner());
	}
}

void ADragonGroundSkill1::AttachMeshToSocket(USceneComponent* InParent, const FName& InSocketName)
{
	FAttachmentTransformRules TransformRules(EAttachmentRule::SnapToTarget, true);
	AttachToComponent(InParent, TransformRules, InSocketName);
}




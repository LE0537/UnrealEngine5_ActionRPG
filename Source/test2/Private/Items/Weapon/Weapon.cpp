// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Weapon/Weapon.h"
#include "Characters/SlashCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Interfaces/HitInterface.h"
#include "NiagaraComponent.h"

AWeapon::AWeapon() : bShowBoxDebug(false), BoxTraceExtent(5.f), BuffMultiplier(1.f)
{
	WeaponBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Weapon Box"));
	WeaponBox->SetupAttachment(GetRootComponent());
	WeaponBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);	// �ݸ����� �����ϴ� �Լ�
	WeaponBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);		// ��� ä�ΰ��� �ݸ��� ��å�� ���������� �ٲٴ� �Լ� SetCollisionResponseTo"AllChannels"
	WeaponBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);		// Pawn���� �ݸ��� ��å�� Ignore�� �ٲ�	SetCollisionResponseTo"Channel"

	BoxTraceStart = CreateDefaultSubobject<USceneComponent>(TEXT("Box Trace Start"));
	BoxTraceStart->SetupAttachment(GetRootComponent());
	
	BoxTraceEnd = CreateDefaultSubobject<USceneComponent>(TEXT("Box Trace End"));
	BoxTraceEnd->SetupAttachment(GetRootComponent());
}

void AWeapon::BeginPlay()
{
	Init();

	WeaponBox->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnBoxOverlap);	// ��������Ʈ �Լ��� �츮�� ���� OnBoxOverlap�� �����ϴ� ��ũ��

}

void AWeapon::Equip(USceneComponent* InParent, FName InSocketName, AActor* NewOwner, APawn* NewInstigator)
{
	ItemState = EItemState::EIS_Equipped;
	SetOwner(NewOwner);				// ��������� Instigator�� ���ϱ� ���� ��� 1: ���� ��ü�� ����(Owner) ����
	SetInstigator(NewInstigator);	// ��������� Instigator�� ���ϱ� ���� ��� 2: �ν�Ƽ������ ����
	AttachMeshToSocket(InParent, InSocketName);		// ������ �޽��� ������ ���Ͽ� �ش� �޽ø� ���̴� �Լ�
	DisableSphereCollision();
	
	//PlayEquipSound();
	DeactivateEmbers();
}

void AWeapon::DeactivateEmbers()
{
	if (ItemEffect)
	{
		ItemEffect->Deactivate();
	}
}

void AWeapon::DisableSphereCollision()
{
	if (Sphere)
	{
		Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);	// ���� �� �������� �ݸ��� ��ü�� ���� �ݸ��� ��ü�� �÷��̾� �� ��ȣ�ۿ��� �Ͼ�� �ʵ��� ��
	}
}

void AWeapon::PlayEquipSound()
{
	if (EquipSound)
	{
		UGameplayStatics::PlaySoundAtLocation(	// ������ ��ġ���� �鸮�� �Ҹ��� �����ؼ� ����ϴ� �Լ�
			this,	// ����ƽ �Լ��� ���忡 ���ؼ� �˾ƾ� �ϴµ�, ȣ���� ��ü ������ �Ѱܵ� ���� ������ �Ѱ��ִ°Ͱ� ����ؼ� this�� �ѱ�
			EquipSound,	// ����� ����
			GetActorLocation()	//����� ��ġ
		);
	}
}

void AWeapon::AttachMeshToSocket(USceneComponent* InParent, const FName& InSocketName)
{
	FAttachmentTransformRules TransformRules(EAttachmentRule::SnapToTarget, true);
	ItemMesh->AttachToComponent(InParent, TransformRules, InSocketName);
}

void AWeapon::BuffOn()
{
	BuffMultiplier = 1.f;
}

void AWeapon::BuffOff()
{
	BuffMultiplier = 1.25f;
}

void AWeapon::Init()
{
	Super::BeginPlay();
}

void AWeapon::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ActorIsSameType(OtherActor))
	{
		return;
	}

	if (OtherActor->ActorHasTag(FName(TEXT("Dead"))))
	{
		return;
	}

	FHitResult BoxHit;
	BoxTrace(BoxHit);
	if (BoxHit.GetActor())
	{
		if (ActorIsSameType(BoxHit.GetActor()))
		{
			return;
		}

		UGameplayStatics::ApplyDamage(
			OtherActor,					// ������� ���� ����, �� ������ ���� �� ������ TakeDamage �Լ��� ȣ��� ��
			Damage * BuffMultiplier,								// �������
			GetInstigator()->GetController(),	// �ν�Ƽ������(������� �� ��ü)�� �÷��̾��� ��Ʈ�ѷ�
			this,								// ������� ���������� �� ��ü�� �ڽ�(����)�� ����
			UDamageType::StaticClass()			// ����� Ÿ��
		);	// ���� Enemy.cpp���� TakeDamage�� ó���Ұ���
		ExecuteGetHit(BoxHit);

		CreateFields(BoxHit.ImpactPoint);
	}
}

bool AWeapon::ActorIsSameType(AActor* OtherActor)
{
	return GetOwner()->ActorHasTag(TEXT("Enemy")) && OtherActor->ActorHasTag(TEXT("Enemy"));
}

void AWeapon::ExecuteGetHit(FHitResult& BoxHit)
{
	IHitInterface* HitInterface = Cast<IHitInterface>(BoxHit.GetActor());

	if (HitInterface)
	{
		//HitInterface->GetHit(BoxHit.ImpactPoint);	// BlueprintNativeEvent�� �Լ� ȣ����� �޶���
		HitInterface->Execute_GetHit(BoxHit.GetActor(), BoxHit.ImpactPoint, GetOwner());	// ���� ������ �����ϴ� �Ű�������� �Բ� �ش� �Լ��� �ִ� ��ü�� �����ؾ� ��
	}
}
void AWeapon::ExecuteGetHitEnemy(AActor* BoxHit)
{
	IHitInterface* HitInterface = Cast<IHitInterface>(BoxHit);

	if (HitInterface)
	{
		//HitInterface->GetHit(BoxHit.ImpactPoint);	// BlueprintNativeEvent�� �Լ� ȣ����� �޶���
		HitInterface->Execute_GetHit(BoxHit, BoxHit->GetActorLocation(), GetOwner());	// ���� ������ �����ϴ� �Ű�������� �Բ� �ش� �Լ��� �ִ� ��ü�� �����ؾ� ��
	}
}
void AWeapon::BoxTrace(FHitResult& BoxHit)
{
	const FVector Start = BoxTraceStart->GetComponentLocation();	// BoxTraceStart�� ��ġ�� �������� �Լ�
	const FVector End = BoxTraceEnd->GetComponentLocation();		// BoxTraceEnd�� ��ġ�� �������� �Լ�

	TArray<AActor*> ActorsToIgnore;	// TArray: �𸮾�5�� ���� �迭

	for (AActor* Actor : IgnoreActors)
	{
		ActorsToIgnore.AddUnique(Actor);
	}

	ActorsToIgnore.Add(this);	// ���� �ڽ�
	ActorsToIgnore.Add(GetOwner());	// �÷��̾�

	UKismetSystemLibrary::BoxTraceSingle(
		this,	// ���� ���ؽ�Ʈ ������Ʈ
		Start,	// ���� ����
		End,	// �� ����
		BoxTraceExtent,					// Ʈ���̽��� ����� �ڽ��� ũ��
		BoxTraceStart->GetComponentRotation(),	// Ʈ���̽��� ����� �ڽ��� ȸ����
		ETraceTypeQuery::TraceTypeQuery1,		// Ʈ���̽����� Ȯ���� ä��
		false,									// bTraceComplex: ������ �浹�� �ƴ� ���� �޽ÿ� �浹�˻縦 �� ������ �����ϴ� �Ű�����
		ActorsToIgnore,							// ������ ���͵��� ��Ƴ��� ���� �迭 ����
		bShowBoxDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,					// �浹�� ����Ǿ��� �� ����� ��ο츦 ��� ���� ����
		BoxHit,									// ����� �浹�� ���� ������ �������� ��¿� �Ű�����
		true									// �浹�� ������ �� �ڱ� �ڽ��� �������� ���ϴ� ����
	);

	IgnoreActors.AddUnique(BoxHit.GetActor());	// Ÿ�� ������ �� ���� �ֱ� ���� AddUnique(�ߺ� ������ ������� �ʴ� Add)
}

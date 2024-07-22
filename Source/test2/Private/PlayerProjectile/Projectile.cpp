// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerProjectile/Projectile.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/BoxComponent.h"
#include "NiagaraComponent.h"
#include "Particles/ParticleSystemComponent.h"

AProjectile::AProjectile()
{
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement"));
	ProjectileMovement->bAutoActivate = false;

	ProjectileMovement->ProjectileGravityScale = 0.001f;
	ProjectileMovement->InitialSpeed = 5000.0f; // �ʱ� �ӵ� ����
	ProjectileMovement->MaxSpeed = 5000.0f; // �ִ� �ӵ� ����
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;
	ProjectileMovement->SetUpdatedComponent(ItemMesh);

	ProjectileBodyParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Body Particle"));
	ProjectileBodyParticle->SetupAttachment(GetRootComponent());
	ProjectileBodyParticle->bAutoActivate = false;

	ProjectileExplosionParticle = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Explosion Particle"));
	FAttachmentTransformRules TransformRules(EAttachmentRule::SnapToTarget, true);
	ProjectileExplosionParticle->AttachToComponent(ItemMesh, TransformRules);
	ProjectileExplosionParticle->bAutoActivate = false;

	Damage = 20.f;
}

void AProjectile::Shoot()
{
	if (ItemMesh)
	{
		ItemMesh->SetSimulatePhysics(true);
		ItemMesh->SetMobility(EComponentMobility::Movable);
	}

	WeaponBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	if (ProjectileMovement)
	{	
		ProjectileMovement->Velocity = GetActorForwardVector() * 1500.f;
		ProjectileMovement->Activate();

		if (ProjectileBodyParticle)
		{
			ProjectileBodyParticle->Activate();
		}
	}

	GetWorldTimerManager().SetTimer(AutoExplosionTimer, this, &AProjectile::PlayExplosionParticle, 3.f);
}

void AProjectile::BeginPlay()
{
	AActor::BeginPlay();

	Tags.Add(TEXT("Skill4Projectile"));

	WeaponBox->OnComponentBeginOverlap.AddDynamic(this, &AProjectile::OnBoxOverlap);
	WeaponBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	if (MeshVector.Num() > 0)
	{
		int32 Selection = FMath::RandRange(0, MeshVector.Num() - 1);
		ItemMesh->SetStaticMesh(MeshVector[Selection]);
	}
}

void AProjectile::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->ActorHasTag(TEXT("Skill4Projectile")))
	{
		return;
	}

	if (OtherActor->ActorHasTag(FName(TEXT("Dead"))))
	{
		return;
	}

	if (OtherActor == nullptr)
	{
		return;
	}
	
	if (OtherActor->ActorHasTag(TEXT("Enemy")))
	{
		UGameplayStatics::ApplyDamage(
			OtherActor,					// ������� ���� ����, �� ������ ���� �� ������ TakeDamage �Լ��� ȣ��� ��
			Damage,								// �������
			GetInstigator()->GetController(),	// �ν�Ƽ������(������� �� ��ü)�� �÷��̾��� ��Ʈ�ѷ�
			this,								// ������� ���������� �� ��ü�� �ڽ�(����)�� ����
			UDamageType::StaticClass()			// ����� Ÿ��
		);	// ���� Enemy.cpp���� TakeDamage�� ó���Ұ���

		ExecuteGetHitEnemy(OtherActor);

		
	}
	//UE_LOG(LogTemp, Warning, TEXT("Weapon %s hit %s !"), *FString(GetName()), *FString(OtherActor->GetName()))
	CreateFields(GetActorLocation());
	PlayExplosionParticle();
}

void AProjectile::PlayExplosionParticle()
{
	if (IsValid(ProjectileExplosionParticle) && IsValid(ProjectileBodyParticle))
	{
		ProjectileExplosionParticle->SetWorldLocation(ItemMesh->GetComponentLocation());
		ProjectileExplosionParticle->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		ProjectileExplosionParticle->SetAutoDestroy(true);
		ProjectileExplosionParticle->ActivateSystem();

		ProjectileBodyParticle->Deactivate();
		Destroy();
	}
}
 
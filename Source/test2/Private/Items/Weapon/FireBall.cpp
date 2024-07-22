// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Weapon/FireBall.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Interfaces/HitInterface.h"
#include "Engine/World.h"

AFireBall::AFireBall()
{
	PrimaryActorTick.bCanEverTick = true;

	FireBallMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FireBallMesh"));
	RootComponent = FireBallMesh;

	WeaponSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Weapon Sphere"));
	WeaponSphere->SetupAttachment(GetRootComponent());

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->bAutoActivate = false;

	ProjectileMovementComponent->ProjectileGravityScale = 0.0f;
	ProjectileMovementComponent->InitialSpeed = 3000.0f; // �ʱ� �ӵ� ����
	ProjectileMovementComponent->MaxSpeed = 3000.0f; // �ִ� �ӵ� ����
}

void AFireBall::BeginPlay()
{
	Super::BeginPlay();
	
	WeaponSphere->OnComponentBeginOverlap.AddDynamic(this, &AFireBall::OnSphereOverlap);
}

void AFireBall::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->ActorHasTag(TEXT("BOSS")))
	{
		return;
	}
	if (OtherActor && (OtherActor != this))
	{
		GEngine->AddOnScreenDebugMessage(1, 5.f, FColor::Red, OtherActor->GetName());

		UGameplayStatics::ApplyDamage(
			OtherActor,					// ������� ���� ����, �� ������ ���� �� ������ TakeDamage �Լ��� ȣ��� ��
			100.f,								// �������
			GetInstigator()->GetController(),	// �ν�Ƽ������(������� �� ��ü)�� �÷��̾��� ��Ʈ�ѷ�
			this,								// ������� ���������� �� ��ü�� �ڽ�(����)�� ����
			UDamageType::StaticClass()			// ����� Ÿ��
		);	// ���� Enemy.cpp���� TakeDamage�� ó���Ұ���

		ExecuteGetHitEnemy(OtherActor);

		WeaponSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		SpawnHitParticles(GetActorLocation());
		//Destroy();
		FireBallMesh->SetHiddenInGame(true);
	}
}

void AFireBall::ExecuteGetHitEnemy(AActor* BoxHit)
{
	IHitInterface* HitInterface = Cast<IHitInterface>(BoxHit);

	if (HitInterface)
	{
		HitInterface->Execute_GetHit(BoxHit, BoxHit->GetActorLocation(), this);
	}
}

void AFireBall::SpawnHitParticles(const FVector& ImpactPoint)
{
	if (HitParticles && GetWorld())
	{
		UGameplayStatics::SpawnEmitterAtLocation(	// Ÿ�� �������� ��ƼŬ ����
			GetWorld(),
			HitParticles,
			ImpactPoint
		);
	}
}

void AFireBall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	SetDead(DeltaTime);
}

void AFireBall::LaunchFireball(const FVector& LaunchVelocity)
{
	if (ProjectileMovementComponent)
	{
		ProjectileMovementComponent->Velocity = LaunchVelocity; // ���� ��ǥ�迡�� �ӵ� ����
		ProjectileMovementComponent->Activate();
	}
}

void AFireBall::SetDead(float DeltaTime)
{
	LifeTime += DeltaTime;
	if (LifeTime >= 5.f)
	{
		SpawnHitParticles(GetActorLocation());
		Destroy();
	}
}


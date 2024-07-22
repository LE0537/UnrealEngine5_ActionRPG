// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Weapon/PoisonBall.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Interfaces/HitInterface.h"
#include "Engine/World.h"

APoisonBall::APoisonBall()
{
	PrimaryActorTick.bCanEverTick = true;

	PoisonBallMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PoisonBallMesh"));
	RootComponent = PoisonBallMesh;

	WeaponSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Weapon Sphere"));
	WeaponSphere->SetupAttachment(GetRootComponent());

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->bAutoActivate = false;

	ProjectileMovementComponent->ProjectileGravityScale = 1.0f;
	ProjectileMovementComponent->InitialSpeed = 3000.0f; // �ʱ� �ӵ� ����
	ProjectileMovementComponent->MaxSpeed = 3000.0f; // �ִ� �ӵ� ����
}

void APoisonBall::BeginPlay()
{
	Super::BeginPlay();

	WeaponSphere->OnComponentBeginOverlap.AddDynamic(this, &APoisonBall::OnSphereOverlap);

}

void APoisonBall::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->ActorHasTag(TEXT("BOSS")))
	{
		return;
	}
	if (OtherActor && (OtherActor != this))
	{
		GEngine->AddOnScreenDebugMessage(1,5.f,FColor::Red, OtherActor->GetName());
		
		UGameplayStatics::ApplyDamage(
			OtherActor,					// ������� ���� ����, �� ������ ���� �� ������ TakeDamage �Լ��� ȣ��� ��
			50.f,								// �������
			GetInstigator()->GetController(),	// �ν�Ƽ������(������� �� ��ü)�� �÷��̾��� ��Ʈ�ѷ�
			this,								// ������� ���������� �� ��ü�� �ڽ�(����)�� ����
			UDamageType::StaticClass()			// ����� Ÿ��
		);	// ���� Enemy.cpp���� TakeDamage�� ó���Ұ���

		ExecuteGetHitEnemy(OtherActor);

		WeaponSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		SpawnHitParticles(GetActorLocation());
		Destroy();

	}
}

void APoisonBall::ExecuteGetHitEnemy(AActor* BoxHit)
{
	IHitInterface* HitInterface = Cast<IHitInterface>(BoxHit);

	if (HitInterface)
	{
		HitInterface->Execute_GetHit(BoxHit, BoxHit->GetActorLocation(), this);
	}
}

void APoisonBall::SpawnHitParticles(const FVector& ImpactPoint)
{
	// Ÿ�� ��ƼŬ ����
	if (HitParticles && GetWorld())
	{
		UGameplayStatics::SpawnEmitterAtLocation(	// Ÿ�� �������� ��ƼŬ ����
			GetWorld(),
			HitParticles,
			ImpactPoint
		);
	}
}

void APoisonBall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	SetDead();
}

void APoisonBall::LaunchFireball(const FVector& LaunchVelocity)
{
	if (ProjectileMovementComponent)
	{
		ProjectileMovementComponent->Velocity = LaunchVelocity; // ���� ��ǥ�迡�� �ӵ� ����
		ProjectileMovementComponent->Activate();
	}
}

void APoisonBall::SetDead()
{
	FVector Location = GetActorLocation();
	if (Location.Z <= 110.f)
	{
		SpawnHitParticles(GetActorLocation());
		Destroy();
	}
}


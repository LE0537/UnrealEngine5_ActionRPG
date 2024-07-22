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
	ProjectileMovementComponent->InitialSpeed = 3000.0f; // 초기 속도 설정
	ProjectileMovementComponent->MaxSpeed = 3000.0f; // 최대 속도 설정
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
			OtherActor,					// 대미지를 받은 액터, 이 정보를 토대로 이 액터의 TakeDamage 함수가 호출될 것
			100.f,								// 대미지값
			GetInstigator()->GetController(),	// 인스티게이터(대미지를 준 주체)인 플레이어의 컨트롤러
			this,								// 대미지를 직접적으로 준 객체인 자신(무기)을 전달
			UDamageType::StaticClass()			// 대미지 타입
		);	// 이제 Enemy.cpp에서 TakeDamage를 처리할것임

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
		UGameplayStatics::SpawnEmitterAtLocation(	// 타격 지점에서 파티클 생성
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
		ProjectileMovementComponent->Velocity = LaunchVelocity; // 월드 좌표계에서 속도 설정
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


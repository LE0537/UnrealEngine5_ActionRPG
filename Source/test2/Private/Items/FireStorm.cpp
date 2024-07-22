// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/FireStorm.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Interfaces/HitInterface.h"
#include "Engine/World.h"

AFireStorm::AFireStorm()
{
	PrimaryActorTick.bCanEverTick = true;

	FireStormMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FireStormMesh"));
	RootComponent = FireStormMesh;
	
	OutDistMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("OutDistMesh"));
	OutDistMesh->SetupAttachment(GetRootComponent());
	InDistMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("InDistMesh"));
	InDistMesh->SetupAttachment(GetRootComponent());

	WeaponSphere = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Weapon Sphere"));
	WeaponSphere->SetupAttachment(GetRootComponent());
	WeaponSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->bAutoActivate = false;

	ProjectileMovementComponent->ProjectileGravityScale = 0.0f;
	ProjectileMovementComponent->InitialSpeed = 3000.0f; // �ʱ� �ӵ� ����
	ProjectileMovementComponent->MaxSpeed = 3000.0f; // �ִ� �ӵ� ����

	MovementDirection = GetRandDiretion();
}

void AFireStorm::BeginPlay()
{
	Super::BeginPlay();
	WeaponSphere->OnComponentBeginOverlap.AddDynamic(this, &AFireStorm::OnCapsuleOverlap);

}

void AFireStorm::OnCapsuleOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
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

		//WeaponSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		SpawnHitParticles(GetActorLocation());
		//Destroy();

	}
}
void AFireStorm::ExecuteGetHitEnemy(AActor* BoxHit)
{
	IHitInterface* HitInterface = Cast<IHitInterface>(BoxHit);

	if (HitInterface)
	{
		HitInterface->Execute_GetHit(BoxHit, BoxHit->GetActorLocation(), this);
	}
}

void AFireStorm::SpawnHitParticles(const FVector& ImpactPoint)
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

void AFireStorm::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CheckLifeTime)
	{
		RandMovement(DeltaTime);
		SetDead(DeltaTime);
	}
	else
	{
		Spawning(DeltaTime);
	}
}

void AFireStorm::HideMesh()
{
	FireStormMesh->SetHiddenInGame(true, true);
	InDistMesh->SetHiddenInGame(false, true);
	OutDistMesh->SetHiddenInGame(false, true);
}

void AFireStorm::SetDead(float DeltaTime)
{
	LifeTime += DeltaTime;
	if (LifeTime >= 5.f)
	{
		Destroy();
	}
}

void AFireStorm::Spawning(float DeltaTime)
{
	if (SpawnTime < 2.1f)
	{
		SpawnTime += DeltaTime;
		float Rate = 15.0f / 2.f;

		InDistScale += Rate * DeltaTime;
		InDistScale = FMath::Clamp(InDistScale, 15.0f, 30.5f);
		InDistMesh->SetRelativeScale3D(FVector(InDistScale, InDistScale, InDistScale));
		if (InDistScale > 30.f)
		{
			CheckLifeTime = true;
			FireStormMesh->SetHiddenInGame(false, true);
			InDistMesh->SetHiddenInGame(true, true);
			OutDistMesh->SetHiddenInGame(true, true);
			WeaponSphere->SetHiddenInGame(true, true);
			WeaponSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		}
	}
}

FVector  AFireStorm::GetRandDiretion()
{
	return FVector(FMath::FRandRange(-1.0f, 1.0f), FMath::FRandRange(-1.0f, 1.0f), 0.f);
}

void AFireStorm::RandMovement(float DeltaTime)
{
	FVector DeltaLocation = MovementDirection * MoveSpeed * DeltaTime;
	FVector NewLocation = GetActorLocation() + DeltaLocation;
	SetActorLocation(NewLocation);

	ResetDirectionTime += DeltaTime;
	if (ResetDirectionTime >= 1.f)
	{
		ResetDirectionTime = 0.f;
		MovementDirection = GetRandDiretion();
	}
}


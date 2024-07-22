// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/BaseCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/AttributeComponent.h"
#include "Items/Weapon/Weapon.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"

ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	Attributes = CreateDefaultSubobject<UAttributeComponent>(TEXT("Attributes"));
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	EquippedWeapon.AddZeroed();
	EquippedWeapon.AddZeroed();
	EquippedWeapon.AddZeroed();
}

void ABaseCharacter::Attack()
{
	if (CombatTarget && CombatTarget->ActorHasTag(FName(TEXT("Dead"))))
	{
		CombatTarget = nullptr;
	}
}

bool ABaseCharacter::CanAttack()
{
	return false;
}

void ABaseCharacter::AttackEnd()
{
}

int32 ABaseCharacter::PlayAttackMontage()
{
	return PlayRandomMontageSection(AttackMontage, AttackMontageSections);
}

bool ABaseCharacter::IsAlive()
{
	return Attributes && Attributes->IsAlive();
}

void ABaseCharacter::Die()
{
	Tags.Add(FName(TEXT("Dead")));
	PlayDeathMontage();
}

void ABaseCharacter::PlayHitReactMontage(const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HitReactMontage)
	{
		AnimInstance->Montage_Play(HitReactMontage);
		AnimInstance->Montage_JumpToSection(SectionName, HitReactMontage);
	}
}

void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABaseCharacter::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	if (IsAlive() && Hitter)
	{
		DirectionalHitReact(Hitter->GetActorLocation());
	}
	else
	{
		Die();
	}

	PlayHitSound(ImpactPoint);
	SpawnHitParticles(ImpactPoint);
}

void ABaseCharacter::SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled)
{
	for (auto& Weapon : EquippedWeapon)
	{
		if (Weapon && Weapon->GetWeaponBox())
		{
			Weapon->GetWeaponBox()->SetCollisionEnabled(CollisionEnabled);
			Weapon->ClearIgnoreActors();
		}
	}

}

void ABaseCharacter::SetMeshCollisionEnabled(ECollisionEnabled::Type CollisionEnabled)
{
	GetMesh()->SetCollisionEnabled(CollisionEnabled);
}

void ABaseCharacter::DirectionalHitReact(const FVector& ImpactPoint)
{
	const FVector Forward = GetActorForwardVector();
	const FVector ImpactLowered(ImpactPoint.X, ImpactPoint.Y, GetActorLocation().Z);
	const FVector ToHit = (ImpactLowered - GetActorLocation()).GetSafeNormal();

	// ���溤�� * Ÿ�� ���� = ���� ���� ���� * Ÿ�� ���� ���� * ���� �� �ڻ���
	const double CosTheta = FVector::DotProduct(Forward, ToHit);
	// �ڻ��� ���Լ�(��ũ�ڻ���)���� ���� ���ϱ�
	double Theta = FMath::Acos(CosTheta);
	// ������ �Ϲ� ������ ��ȯ
	Theta = FMath::RadiansToDegrees(Theta);

	// ������ �Ʒ��� ����Ų�ٸ�, ��Ÿ���� ������ �Ǿ�� �Ѵ�
	const FVector CrossProduct = FVector::CrossProduct(Forward, ToHit);
	if (CrossProduct.Z < 0.f)
	{
		Theta *= -1.f;
	}

	FName Section(TEXT("HitBack"));

	if (Theta >= -45.f && Theta < 45.f)
	{
		Section = FName(TEXT("HitFront"));
	}
	else if (Theta >= -135.f && Theta < -45.f)
	{
		Section = FName(TEXT("HitLeft"));
	}
	else if (Theta >= 45.f && Theta < 135.f)
	{
		Section = FName(TEXT("HitRight"));
	}

	PlayHitReactMontage(Section);
}

void ABaseCharacter::HitReact(const FVector& ImpactPoint, AActor* Hitter)
{
	//const FVector Location = GetActorLocation();
	//const FVector HitterLocation = Hitter->GetActorLocation();
	//const FVector Forward = (HitterLocation - Location).GetSafeNormal();
	//const FRotator NewRotation = UKismetMathLibrary::MakeRotFromX(Forward);
	//SetActorRotation(NewRotation);
	FName Section(TEXT("Hit1"));
	if (isCritical)
	{
		Section = FName(TEXT("Hit2"));
	}

	PlayHitReactMontage(Section);
}

void ABaseCharacter::PlayHitSound(const FVector& ImpactPoint)
{
	// Ÿ�� ���� ���
	if (HitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(	// Ÿ�� �������� ���� �÷���
			this,
			HitSound,
			ImpactPoint
		);
	}
}

void ABaseCharacter::SpawnHitParticles(const FVector& ImpactPoint)
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

void ABaseCharacter::HandleDamage(float DamageAmount)
{
	if (Attributes)
	{
		Attributes->ReceiveDamage(DamageAmount);
	}
}

void ABaseCharacter::PlayMontageSection(UAnimMontage* Montage, const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && Montage)
	{
		AnimInstance->Montage_Play(Montage);
		AnimInstance->Montage_JumpToSection(SectionName, Montage);
	}
}

int32 ABaseCharacter::PlayRandomMontageSection(UAnimMontage* Montage, const TArray<FName>& SectionNames)
{
	if (SectionNames.Num() <= 0) return -1;
	const int32 MaxSectionIndex = SectionNames.Num() - 1;
	const int32 Selection = FMath::RandRange(0, MaxSectionIndex);
	PlayMontageSection(Montage, SectionNames[Selection]);
	return Selection;
}

int32 ABaseCharacter::PlayDeathMontage()
{
	const int32 Selection = PlayRandomMontageSection(DeathMontage, DeathMontageSections);
	TEnumAsByte<EDeathPose> Pose(Selection);
	if (Pose < EDeathPose::EDP_MAX)
	{
		DeathPose = Pose;
	}

	return Selection;
}

void ABaseCharacter::StopAttackMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		AnimInstance->Montage_Stop(0.25f, AttackMontage);
	}
}

FVector ABaseCharacter::GetTranslationWarpTarget()
{
	if (CombatTarget == nullptr)
	{
		return FVector();
	}
	
	const FVector CombatTargetLocation = CombatTarget->GetActorLocation();	// ��ǥ ����� ��ġ
	const FVector Location = GetActorLocation();							// ���� ������ ��ġ

	FVector TargetToMe = (Location - CombatTargetLocation).GetSafeNormal();	// ��ǥ ��󿡼� ������ ���� ���⺤��(����ȭ��)
	TargetToMe *= WarpTargetDistance;										// ��ǥ ��������ŭ ���⺤�Ϳ� ����

	return CombatTargetLocation + TargetToMe;								// ��ǥ���Լ� ��ǥ ��������ŭ ������ ��ġ�� ���� ��ȯ��
}

FVector ABaseCharacter::GetRotationWarpTarget()
{
	if (CombatTarget)
	{
		return CombatTarget->GetActorLocation();
	}
	return FVector();
}

void ABaseCharacter::DisableCapsule()
{
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ABaseCharacter::DisableMeshCollision()
{
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

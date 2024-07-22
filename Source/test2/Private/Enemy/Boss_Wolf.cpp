// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Boss_Wolf.h"
#include "Items/Weapon/Weapon.h"
#include "Kismet/KismetMathLibrary.h"
#include "Items/Weapon/PoisonBall.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Perception/PawnSensingComponent.h"
#include "Components/AttributeComponent.h"
#include "HUD/BossHUD.h"
#include "Characters/SlashCharacter.h"
#include "BGMManager/BGMManager.h"

void ABoss_Wolf::BeginPlay()
{
	Super::BeginPlay();
	ChasingSpeed = 600.f;


	PawnSensing->SightRadius = 1.f;

	PlaySpawnMontage();
}

void ABoss_Wolf::Die()
{
	Super::Die();
}

void ABoss_Wolf::Attack()
{
	if (EnemyState == EEnemyState::EES_Dead || EnemyState == EEnemyState::EES_Skill1 || EnemyState == EEnemyState::EES_Skill2)
	{
		ClearAttackTimer();
		return;
	}
	Super::Attack();
}

bool ABoss_Wolf::CanAttack()
{
	return Super::CanAttack();
}

void ABoss_Wolf::HandleDamage(float DamageAmount)
{
	if (Attributes)
	{
		Attributes->ReceiveDamage(DamageAmount);
	}
	if (Attributes && BossHUD)
	{
		BossHUD->SetHealthPercent(Attributes->GetHealthPercent());
	}
}

int32 ABoss_Wolf::PlayDeathMontage()
{
	return Super::PlayDeathMontage();
}

void ABoss_Wolf::AttackEnd()
{
	GetCharacterMovement()->MaxWalkSpeed = ChasingSpeed;
	EnemyState = EEnemyState::EES_NoState;
	ChaseCombatTarget();
}

void ABoss_Wolf::SpawnPoisonBall()
{
	LaunchPoison();
}

void ABoss_Wolf::SpawnCutSceneEnd()
{
	SpawnCutScene = false;
	PawnSensing->SightRadius = 10000.f;

	UWorld* World = GetWorld();

	if (World)
	{
		APlayerController* P_Controller = World->GetFirstPlayerController();
		ASlashCharacter* Player = Cast<ASlashCharacter>(P_Controller->GetPawn());

		if (P_Controller && Player)
		{
			Player->AddQuest(FString(TEXT("거미 사냥")), FString(TEXT("거미를 사냥하십시오.")));
			Player->GetBGMManager()->StopBGM();
			Player->GetBGMManager()->PlayBGM(EBGM::EBGM_Spider);
		}

		if (P_Controller && BossHUDClass)
		{
			BossHUD = CreateWidget<UBossHUD>(P_Controller, BossHUDClass);
			BossHUD->AddToViewport();
			FString Name = "Giant Spider(BOSS)";
			BossHUD->SetName(Name);
			BossHUD->SetHealthPercent(1.f);
		}
	}
}

void ABoss_Wolf::SpawnCutSceneShowMesh()
{
	//FTimerHandle TimerHandle;
	//GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ABoss_Wolf::ShowMesh, 0.4f, false);
	ShowMesh();

}



void ABoss_Wolf::ChaseCombatTarget()
{
	if (bSkillReserve[0])
	{
		Boss_Skill1();
		return;
	}
	else if (bSkillReserve[1])
	{
		Boss_Skill2();
		return;
	}

	 if (IsOutsideAttackRadius() && !IsChasing())
	{
		// 공격 타이머 초기화
		ClearAttackTimer();
		// 공격 범위 밖이면 주인공을 추적함
		if (!IsEngaged() &&
			EnemyState != EEnemyState::EES_Skill1 &&
			EnemyState != EEnemyState::EES_Skill2)
		{
			ChaseTarget();
		}
	
	}
	else if (CanAttack() && EnemyState != EEnemyState::EES_Skill1 &&
		EnemyState != EEnemyState::EES_Skill2 && !bSkillReserve[0] && !bSkillReserve[1])
	{
		StartAttackTimer();
	}
	 if (!bSkillStart[0] && EnemyState != EEnemyState::EES_Skill1 && 
		 EnemyState != EEnemyState::EES_Skill2)
	 {
		 StartSkill1Timer();
	 }
	 if (!bSkillStart[1] && EnemyState != EEnemyState::EES_Skill1 &&
		 EnemyState != EEnemyState::EES_Skill2)
	 {
		 StartSkill2Timer();
	 }
}



void ABoss_Wolf::Tick(float DeltaTime)
{
	
	if (!SpawnCutScene)
	{
		ABaseCharacter::Tick(DeltaTime);
		if (IsDead())
		{
			return;
		}

		if (EnemyState > EEnemyState::EES_Patrolling)
		{
			ChaseCombatTarget();
		}
	}

}

void ABoss_Wolf::Destroyed()
{
	Super::Destroy();
}

void ABoss_Wolf::StartSkill1Timer()
{
	bSkillStart[0] = true;
	const float Skill1Time = 10.f;
	GetWorldTimerManager().SetTimer(Skill1Timer, this, &ABoss_Wolf::Boss_Skill1, Skill1Time);
}

void ABoss_Wolf::ClearSkill1Timer()
{
	GetWorldTimerManager().ClearTimer(Skill1Timer);
}

void ABoss_Wolf::StartSkill2Timer()
{
	bSkillStart[1] = true;
	const float Skill2Time = 12.f;
	GetWorldTimerManager().SetTimer(Skill2Timer, this, &ABoss_Wolf::Boss_Skill2, Skill2Time);
}

void ABoss_Wolf::ClearSkill2Timer()
{
	GetWorldTimerManager().ClearTimer(Skill2Timer);
}

void ABoss_Wolf::Boss_Skill1()
{
	if (EnemyState == EEnemyState::EES_Attacking || EnemyState == EEnemyState::EES_Engaged ||
		EnemyState == EEnemyState::EES_Dead || EnemyState == EEnemyState::EES_Skill2)
	{
		bSkillReserve[0] = true;
		return;
	}

	ABaseCharacter::Attack();
	if (CombatTarget == nullptr)
	{
		return;
	}
	StopMove();
	FVector TargetPos = CombatTarget->GetActorLocation();
	TargetPos.Z = GetActorLocation().Z;
	const FVector Direction = (TargetPos - GetActorLocation()).GetSafeNormal();
	
	const FRotator NewRotation = UKismetMathLibrary::MakeRotFromX(Direction);
	SetActorRotation(NewRotation);
	ClearAttackTimer();
	ClearSkill1Timer();
	EnemyState = EEnemyState::EES_Skill1;
	bSkillStart[0] = false;
	bSkillReserve[0] = false;
	PlaySkillMontage(SkillMontage, SkillMontageSections[0]);
}
void ABoss_Wolf::Boss_Skill2()
{
	if (EnemyState == EEnemyState::EES_Attacking || EnemyState == EEnemyState::EES_Engaged || 
		EnemyState == EEnemyState::EES_Dead || EnemyState == EEnemyState::EES_Skill1)
	{
		bSkillReserve[1] = true;
		return;
	}

	ABaseCharacter::Attack();
	if (CombatTarget == nullptr)
	{
		return;
	}
	StopMove();
	GetCharacterMovement()->MaxWalkSpeed = 0.f;
	FVector TargetPos = CombatTarget->GetActorLocation();
	TargetPos.Z = GetActorLocation().Z;
	const FVector Direction = (TargetPos - GetActorLocation()).GetSafeNormal();

	const FRotator NewRotation = UKismetMathLibrary::MakeRotFromX(Direction);
	SetActorRotation(NewRotation);
	ClearAttackTimer();
	ClearSkill2Timer();
	EnemyState = EEnemyState::EES_Skill2;
	bSkillStart[1] = false;
	bSkillReserve[1] = false;
	PlaySkillMontage(Skill2Montage, SkillMontageSections[0]);
}

void ABoss_Wolf::ShowMesh()
{
	GetMesh()->SetVisibility(true);
}


void ABoss_Wolf::LaunchPoison()
{
	if (PoisonBallClass)
	{
		// RightHandSocket 위치 가져오기
		USkeletalMeshComponent* MeshComp = GetMesh();
		if (MeshComp)
		{
			FVector SocketLocation = MeshComp->GetSocketLocation(FName("SpawnPoisonBall"));
			FRotator SocketRotation = MeshComp->GetSocketRotation(FName("SpawnPoisonBall"));

			APoisonBall* Poisonball = GetWorld()->SpawnActor<APoisonBall>(PoisonBallClass, SocketLocation, SocketRotation);

			if (Poisonball)
			{
				// 10 ~ 50m 범위 내에서 전방 거리 설정
				float RandomDistanceForward = FMath::RandRange(100.0f, 1000.0f);
				// 좌우 -50 ~ 50m 범위 내에서 랜덤 좌표 설정
				float RandomDistanceSideways = FMath::RandRange(-700.0f, 700.0f);

				// 전방 방향과 좌우 방향 벡터를 구해서 더함
				FVector LaunchDirection = GetActorForwardVector() * RandomDistanceForward + GetActorRightVector() * RandomDistanceSideways;
	
				// 포물선을 그리기 위한 Z축 속도 추가
				LaunchDirection.Z += 1500.0f; // Z축 속도를 적절히 설정
				// 발사 속도 설정
				FVector LaunchVelocity = LaunchDirection;

				Poisonball->LaunchFireball(LaunchVelocity);
				Poisonball->SetInstigator(this);
			}
		}
	}
}

void ABoss_Wolf::PlaySkillMontage(UAnimMontage* Montage, const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && Montage)
	{
		AnimInstance->Montage_Play(Montage);
		AnimInstance->Montage_JumpToSection(SectionName, Montage);
	}
}

void ABoss_Wolf::PlaySpawnMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && SpawnMontage)
	{
		AnimInstance->Montage_Play(SpawnMontage);
		AnimInstance->Montage_JumpToSection(SpawnMontageSections, SpawnMontage);
	}
}

float ABoss_Wolf::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	HandleDamage(DamageAmount);
	CombatTarget = EventInstigator->GetPawn();
	if (IsInAttackRadius() && EnemyState != EEnemyState::EES_Skill1)
	{
		EnemyState = EEnemyState::EES_Attacking;
	}
	else if (IsOutsideAttackRadius() && EnemyState != EEnemyState::EES_Skill1)
	{
		ChaseTarget();
	}
	return DamageAmount;
}

void ABoss_Wolf::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	if (IsAlive() && Hitter && EnemyState != EEnemyState::EES_Attacking && 
		EnemyState != EEnemyState::EES_Engaged && 
		EnemyState != EEnemyState::EES_Skill1)
	{
		HitReact(Hitter->GetActorLocation(), Hitter);
		ClearAttackTimer();

	}
	else if (!IsAlive())
	{
		BossHUD->RemoveFromViewport();
		ASlashCharacter* Player = Cast<ASlashCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
		if (Player)
		{
			Player->UpdateQuest(FString(TEXT("거미 사냥")));
			Player->GetBGMManager()->StopBGM();
			Player->GetBGMManager()->PlayBGM(EBGM::EBGM_Base);
		}
		Die();
	}

	PlayHitSound(ImpactPoint);
	SpawnHitParticles(ImpactPoint);

	ClearPatrolTimer();
}



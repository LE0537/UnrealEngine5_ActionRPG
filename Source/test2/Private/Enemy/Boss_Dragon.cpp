// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Boss_Dragon.h"
#include "Items/Weapon/Weapon.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Perception/PawnSensingComponent.h"
#include "HUD/HealthBarComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TimerManager.h"
#include "Components/AttributeComponent.h"
#include "HUD/BossHUD.h"
#include "Items/DragonGroundSkill1.h"
#include "Items/Weapon/FireBall.h"
#include "Items/FireStorm.h"
#include "Characters/SlashCharacter.h"
#include "BGMManager/BGMManager.h"

ABoss_Dragon::ABoss_Dragon()
{
}

void ABoss_Dragon::BeginPlay()
{
	Super::BeginPlay();

	UWorld* World = GetWorld();

	ADragonGroundSkill1* Skill = World->SpawnActor<ADragonGroundSkill1>(GroundSkill1Class);
	Skill->SetSocket(GetMesh(), FName(TEXT("GroundSkill1")), this, this);
	GroundSkill1 = Skill;

	ChasingSpeed = 700.f;

	GetCharacterMovement()->GravityScale = 0.f;
	PawnSensing->SightRadius = 1.f;

	FVector Loc = GetActorLocation();
	Loc.Z += CurrentValue;
	SetActorLocation(Loc);
	PlaySpawnMontage();
}

void ABoss_Dragon::Die()
{
	Super::Die();
}

void ABoss_Dragon::Attack()
{
	if (EnemyState == EEnemyState::EES_Dead || EnemyState == EEnemyState::EES_Skill1 || EnemyState == EEnemyState::EES_Skill2)
	{
		ClearAttackTimer();
		return;
	}
	Super::Attack();
}

bool ABoss_Dragon::CanAttack()
{
	return Super::CanAttack();
}

void ABoss_Dragon::HandleDamage(float DamageAmount)
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

int32 ABoss_Dragon::PlayDeathMontage()
{
	return Super::PlayDeathMontage();
}

void ABoss_Dragon::AttackEnd()
{
	GetCharacterMovement()->MaxWalkSpeed = ChasingSpeed;
	EnemyState = EEnemyState::EES_NoState;
	ChaseCombatTarget();
}

void ABoss_Dragon::SetGroundSkill1(ECollisionEnabled::Type CollisionEnabled)
{
	if (GroundSkill1)
	{
		GroundSkill1->SetGroundSkill(CollisionEnabled);
	}
}

void ABoss_Dragon::SpawnCutSceneEnd()
{
	GetCharacterMovement()->GravityScale = 1.f;
	SpawnCutScene = false;
	PawnSensing->SightRadius = 10000.f;

	UWorld* World = GetWorld();

	if (World)
	{
		APlayerController* P_Controller = World->GetFirstPlayerController();
		ASlashCharacter* Player = Cast<ASlashCharacter>(P_Controller->GetPawn());

		if (Player)
		{
			Player->UpdateQuest(FString(TEXT("언데드의 원흉 찾기")));
			Player->AddQuest(FString(TEXT("용 사냥")), FString(TEXT("용을 처치하십시오.")));
			Player->GetBGMManager()->StopBGM();
			Player->GetBGMManager()->PlayBGM(EBGM::EBGM_Dragon);
		}

		if (P_Controller && BossHUDClass)
		{
			BossHUD = CreateWidget<UBossHUD>(P_Controller, BossHUDClass);
			BossHUD->AddToViewport();
			FString Name = "Elder Dragon(BOSS)";
			BossHUD->SetName(Name);
			BossHUD->SetHealthPercent(1.f);
		}
	}
}

void ABoss_Dragon::SpawnStartFail()
{
	const float Duration = 1.7f;
	const float TargetValue = 0.0f;
	const float InitialValue = CurrentValue;

	// Calculate the total number of steps
	const int32 TotalSteps = 100; // You can adjust the number of steps for smoothness
	const float StepTime = Duration / TotalSteps;
	const float DeltaValue = (InitialValue - TargetValue) / TotalSteps;

	Steps = TotalSteps;

	GetWorld()->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([this, DeltaValue]() mutable {
		if (Steps > 0)
		{
			CurrentValue -= DeltaValue;
			Steps--;

			if (Steps == 0)
			{
				CurrentValue = 0.0f;
				GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
			}
		}
		}), StepTime, true);
}



void ABoss_Dragon::ChaseCombatTarget()
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
	else if (bSkillReserve[2] && FlyCoolTime <=0.f)
	{
		Boss_Skill3();
		return;
	}
	else if (bSkillReserve[3] && FlyCoolTime <= 0.f)
	{
		Boss_Skill4();
		return;
	}


	if (IsOutsideAttackRadius() && !IsChasing())
	{
		// 공격 타이머 초기화
		ClearAttackTimer();
		// 공격 범위 밖이면 주인공을 추적함
		if (!IsEngaged() &&
			EnemyState != EEnemyState::EES_Skill1 &&
			EnemyState != EEnemyState::EES_Skill2 &&
			EnemyState != EEnemyState::EES_Skill3 &&
			EnemyState != EEnemyState::EES_Skill4)
		{
			ChaseTarget();
		}

	}
	else if (CanAttack() && EnemyState != EEnemyState::EES_Skill1 &&
		EnemyState != EEnemyState::EES_Skill2 && EnemyState != EEnemyState::EES_Skill3 &&
		EnemyState != EEnemyState::EES_Skill4 && !bSkillReserve[0] &&
		!bSkillReserve[1] && !bSkillReserve[2] && !bSkillReserve[3])
	{
		StartAttackTimer();
	}
	if (!bSkillStart[0] && EnemyState != EEnemyState::EES_Skill1 &&
		EnemyState != EEnemyState::EES_Skill2 &&
		EnemyState != EEnemyState::EES_Skill3 &&
		EnemyState != EEnemyState::EES_Skill4)
	{
		StartSkill1Timer();
	}
	if (!bSkillStart[1] && EnemyState != EEnemyState::EES_Skill1 &&
		EnemyState != EEnemyState::EES_Skill2 &&
		EnemyState != EEnemyState::EES_Skill3 &&
		EnemyState != EEnemyState::EES_Skill4)
	{
		StartSkill2Timer();
	}
	if (!bSkillStart[2] && EnemyState != EEnemyState::EES_Skill1 &&
		EnemyState != EEnemyState::EES_Skill2 &&
		EnemyState != EEnemyState::EES_Skill3 &&
		EnemyState != EEnemyState::EES_Skill4 && FlyCoolTime <= 0.f)
	{
		StartSkill3Timer();
	}
	if (!bSkillStart[3] && EnemyState != EEnemyState::EES_Skill1 &&
		EnemyState != EEnemyState::EES_Skill2 &&
		EnemyState != EEnemyState::EES_Skill3 &&
		EnemyState != EEnemyState::EES_Skill4 && FlyCoolTime <= 0.f)
	{
		StartSkill4Timer();
	}
}

void ABoss_Dragon::Tick(float DeltaTime)
{
	CheckGround();

	if (!SpawnCutScene)
	{
		if (FlyCoolTime > 0.f)
		{
			FlyCoolTime -= DeltaTime;
			if (FlyCoolTime < 0) FlyCoolTime = 0.f;
		}

		ABaseCharacter::Tick(DeltaTime);
		if (IsDead())
		{
			return;
		}

		if (EnemyState > EEnemyState::EES_Patrolling && GetCharacterMovement()->GravityScale > 0.f)
		{
			ChaseCombatTarget();
		}
	}

}

void ABoss_Dragon::Destroyed()
{
	Super::Destroy();
}

void ABoss_Dragon::StartSkill1Timer()
{
	bSkillStart[0] = true;
	const float Skill1Time = 12.f;
	GetWorldTimerManager().SetTimer(Skill1Timer, this, &ABoss_Dragon::Boss_Skill1, Skill1Time);
}

void ABoss_Dragon::ClearSkill1Timer()
{
	GetWorldTimerManager().ClearTimer(Skill1Timer);
}

void ABoss_Dragon::StartSkill2Timer()
{
	bSkillStart[1] = true;
	const float Skill2Time = 11.f;
	GetWorldTimerManager().SetTimer(Skill2Timer, this, &ABoss_Dragon::Boss_Skill2, Skill2Time);
}

void ABoss_Dragon::ClearSkill2Timer()
{
	GetWorldTimerManager().ClearTimer(Skill2Timer);
}

void ABoss_Dragon::StartSkill3Timer()
{
	bSkillStart[2] = true;
	const float Skill3Time = 20.f;
	GetWorldTimerManager().SetTimer(Skill3Timer, this, &ABoss_Dragon::Boss_Skill3, Skill3Time);
}

void ABoss_Dragon::ClearSkill3Timer()
{
	GetWorldTimerManager().ClearTimer(Skill3Timer);
}

void ABoss_Dragon::StartSkill4Timer()
{
	bSkillStart[3] = true;
	const float Skill4Time = 13.f;
	GetWorldTimerManager().SetTimer(Skill4Timer, this, &ABoss_Dragon::Boss_Skill4, Skill4Time);
}

void ABoss_Dragon::ClearSkill4Timer()
{
	GetWorldTimerManager().ClearTimer(Skill4Timer);
}

void ABoss_Dragon::Boss_Skill1()
{
	if (EnemyState == EEnemyState::EES_Attacking || EnemyState == EEnemyState::EES_Engaged ||
		EnemyState == EEnemyState::EES_Dead || EnemyState == EEnemyState::EES_Skill2 ||
		EnemyState == EEnemyState::EES_Skill3 || EnemyState == EEnemyState::EES_Skill4)
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
void ABoss_Dragon::Boss_Skill2()
{
	if (EnemyState == EEnemyState::EES_Attacking || EnemyState == EEnemyState::EES_Engaged ||
		EnemyState == EEnemyState::EES_Dead || EnemyState == EEnemyState::EES_Skill1 ||
		EnemyState == EEnemyState::EES_Skill3 || EnemyState == EEnemyState::EES_Skill4)
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

void ABoss_Dragon::Boss_Skill3()
{

	if (EnemyState == EEnemyState::EES_Attacking || EnemyState == EEnemyState::EES_Engaged ||
		EnemyState == EEnemyState::EES_Dead || EnemyState == EEnemyState::EES_Skill1 ||
		EnemyState == EEnemyState::EES_Skill2 || EnemyState == EEnemyState::EES_Skill4)
	{
		bSkillReserve[2] = true;
		return;
	}
	if (bFlying) return;
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
	ClearSkill3Timer();
	EnemyState = EEnemyState::EES_Skill3;
	bFlying = true;
	bSkillStart[2] = false;
	bSkillReserve[2] = false;
	GetCharacterMovement()->GravityScale = 0.f;
	StartFly(2.f, FlyTargetValue);
}

void ABoss_Dragon::Boss_Skill4()
{

	if (EnemyState == EEnemyState::EES_Attacking || EnemyState == EEnemyState::EES_Engaged ||
		EnemyState == EEnemyState::EES_Dead || EnemyState == EEnemyState::EES_Skill1 ||
		EnemyState == EEnemyState::EES_Skill2 || EnemyState == EEnemyState::EES_Skill3)
	{
		bSkillReserve[3] = true;
		return;
	}
	if (bFlying) return;
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
	ClearSkill4Timer();
	EnemyState = EEnemyState::EES_Skill4;
	bFlying = true;
	bSkillStart[3] = false;
	bSkillReserve[3] = false;
	GetCharacterMovement()->GravityScale = 0.f;
	StartFly(0.75f, FlySkill2Value);
}

void ABoss_Dragon::ShowMesh()
{
	GetMesh()->SetVisibility(true);
}

void ABoss_Dragon::StartFly(float _duration, float Value)
{
	const float Duration = _duration;
	const float InitialValue = CurrentValue;

	const int32 TotalSteps = 100;
	const float StepTime = Duration / TotalSteps;
	const float DeltaValue = (Value - InitialValue) / TotalSteps;

	Steps = TotalSteps;

	GetWorld()->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([this, DeltaValue, Value]() mutable {
		if (Steps > 0)
		{
			CurrentValue += DeltaValue;
			Steps--;

			if (Steps == 0)
			{
				CurrentValue = Value;
				GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
				if (EnemyState == EEnemyState::EES_Skill3) PlaySkillMontage(Skill3Montage, SkillMontageSections[0]);
				else if (EnemyState == EEnemyState::EES_Skill4) PlaySkillMontage(Skill4Montage, SkillMontageSections[0]);
			}
		}
		}), StepTime, true);
}

void ABoss_Dragon::EndFly(float _duration)
{
	const float Duration = _duration;
	const float TargetValue = 0.0f;
	const float InitialValue = CurrentValue;

	const int32 TotalSteps = 100;
	const float StepTime = Duration / TotalSteps;
	const float DeltaValue = (InitialValue - TargetValue) / TotalSteps;

	EndSteps = TotalSteps;

	GetWorld()->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([this, DeltaValue]() mutable {
		if (EndSteps > 0)
		{
			CurrentValue -= DeltaValue;
			EndSteps--;

			if (EndSteps == 0)
			{
				CurrentValue = 0.0f;
				GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
				GetCharacterMovement()->GravityScale = 1.f;
				bFlying = false;
				FlyCoolTime = 2.f;
				AttackEnd();
			}
		}
		}), StepTime, true);
}

void ABoss_Dragon::CheckGround()
{
	if (GetCharacterMovement()->GravityScale <= 0.f)
	{
		FVector Loc = GetActorLocation();
		Loc.Z = CurrentValue + GroudZ;
		SetActorLocation(Loc);
	}
}

void ABoss_Dragon::AttachMeshToSocket(const FName& InSocketName)
{
	FAttachmentTransformRules TransformRules(EAttachmentRule::SnapToTarget, true);
	GetMesh()->AttachToComponent(GetMesh(), TransformRules, InSocketName);
}

void ABoss_Dragon::LaunchFireBall()
{
	if (FireBallClass)
	{
		// RightHandSocket 위치 가져오기
		USkeletalMeshComponent* MeshComp = GetMesh();
		if (MeshComp)
		{
			FVector SocketLocation = MeshComp->GetSocketLocation(FName("HeadSocket"));
			FRotator SocketRotation = MeshComp->GetSocketRotation(FName("HeadSocket"));

			AFireBall* Fireball = GetWorld()->SpawnActor<AFireBall>(FireBallClass, SocketLocation, SocketRotation);

			if (Fireball)
			{
				FVector TargetLoc = CombatTarget->GetActorLocation();
				TargetLoc.Z += 120.f;
				// 전방 방향과 좌우 방향 벡터를 구해서 더함
				FVector LaunchDirection = (TargetLoc - GetActorLocation()).GetSafeNormal();

				// 발사 속도 설정
				FVector LaunchVelocity = LaunchDirection * 2500.f;

				Fireball->LaunchFireball(LaunchVelocity);
				Fireball->SetInstigator(this);
			}
		}
	}

}

void ABoss_Dragon::SpawningFireStorm()
{
	if (FireStormClass)
	{
		for (int i = 0; i < 25; ++i)
		{
			float RandX = FMath::RandRange(-3500.0f, 3500.0f);
			float RandY = FMath::RandRange(-3500.0f, 3500.0f);
			FVector loc = { FireStormSpawnLoc.X + RandX, FireStormSpawnLoc.Y + RandY, FireStormSpawnLoc.Z + 5.f };
			FRotator SpawnRotation = FRotator::ZeroRotator;

			AFireStorm* FireStorm = GetWorld()->SpawnActor<AFireStorm>(FireStormClass, loc, SpawnRotation);
			FireStorm->SetInstigator(this);
			FireStorm->HideMesh();
		}
	}
}

void ABoss_Dragon::PlaySkillMontage(UAnimMontage* Montage, const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && Montage)
	{
		AnimInstance->Montage_Play(Montage);
		AnimInstance->Montage_JumpToSection(SectionName, Montage);
	}
}

void ABoss_Dragon::PlaySpawnMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && SpawnMontage)
	{
		AnimInstance->Montage_Play(SpawnMontage);
		AnimInstance->Montage_JumpToSection(SpawnMontageSections, SpawnMontage);
	}
}

float ABoss_Dragon::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
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

void ABoss_Dragon::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
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
		ASlashCharacter* Player = Cast<ASlashCharacter>(Hitter);
		if (Player)
		{
			Player->UpdateQuest(FString(TEXT("용 사냥")));
			Player->GetBGMManager()->StopBGM();
			Player->GetBGMManager()->PlayBGM(EBGM::EBGM_Base);
		}
		Die();
	}

	PlayHitSound(ImpactPoint);
	SpawnHitParticles(ImpactPoint);

	ClearPatrolTimer();
}
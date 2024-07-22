// Fill out your copyright notice in the Description page of Project Settings.

// ����� ���̴� ���� ������ �ð� ���࿡ ������ ��
#include "Enemy/Enemy.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/AttributeComponent.h"
#include "HUD/HealthBarComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "Perception/PawnSensingComponent.h"
#include "Items/Weapon/Weapon.h"
#include "Kismet/KismetMathLibrary.h"
#include "Characters/SlashCharacter.h"
#include "Items/Soul.h"

// Sets default values
AEnemy::AEnemy() : CombatRadius(500.f), PatrolRadius(200.f), EnemyState(EEnemyState::EES_Patrolling), AttackRadius(150.f), PatrollingSpeed(125.f), ChasingSpeed(300.f)
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(true);

	HealthBarWidget = CreateDefaultSubobject<UHealthBarComponent>(TEXT("HealthBar"));
	HealthBarWidget->SetupAttachment(GetRootComponent());

	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;


	PawnSensing = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensing"));

	PawnSensing->SetPeripheralVisionAngle(45.f);

	GetCharacterMovement()->MaxWalkSpeed = 125.f;

}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (IsDead())
	{
		return;
	}

	if (EnemyState > EEnemyState::EES_Patrolling)
	{
		CheckCombatTarget();
	}
	else
	{
		CheckPatrolTarget();
	}
}

void AEnemy::Destroyed()
{
	if (EquippedWeapon.Num() > 0)
	{
		for (auto& Weapon : EquippedWeapon)
		{
			if(Weapon) Weapon->Destroy();
		}
		EquippedWeapon.Empty();
	}
}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	HandleDamage(DamageAmount);
	CombatTarget = EventInstigator->GetPawn();
	if (IsInAttackRadius())
	{
		EnemyState = EEnemyState::EES_Attacking;
	}
	else if (IsOutsideAttackRadius())
	{
		ChaseTarget();
	}
	return DamageAmount;
}

void AEnemy::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	if (IsAlive() && Hitter && EnemyState != EEnemyState::EES_Attacking && EnemyState != EEnemyState::EES_Engaged)
	{
		HitReact(Hitter->GetActorLocation(), Hitter);
		ClearAttackTimer();
		SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);
		StopAttackMontage();
	}
	else if(!IsAlive())
	{
		Die();


		if (Hitter->ActorHasTag(FName(TEXT("EngageableTarget"))) && ActorHasTag(FName(TEXT("UndeadEnemy"))))
		{
			UE_LOG(LogTemp, Warning, TEXT("Player found and UndeadEnemy Tag Found"))
			ASlashCharacter* Player = Cast<ASlashCharacter>(Hitter);
			FString QuestNameString = FString(TEXT("마을의 언데드 사냥"));

			if (Player->IsQuestActive(QuestNameString))
			{
				UE_LOG(LogTemp, Warning, TEXT("Quest Name Found"))
				UWorld* World = GetWorld();

				if (World)
				{
					UE_LOG(LogTemp, Warning, TEXT("World is not null"))
					FString BlueprintPath = TEXT("/Game/Blueprints/Item/Souls/BP_Soul.BP_Soul_C");
					UClass* BlueprintClass = Cast<UClass>(StaticLoadObject(UClass::StaticClass(), nullptr, *FName(*BlueprintPath).ToString()));

					if (BlueprintClass)
					{
						UE_LOG(LogTemp, Warning, TEXT("Blueprint class created"))
						FActorSpawnParameters ActorSpawnParameters;

						ASoul* NewSoul = World->SpawnActor<ASoul>(BlueprintClass, GetActorLocation(), GetActorRotation(), ActorSpawnParameters);
						if (NewSoul)
						{
							UE_LOG(LogTemp, Warning, TEXT("Soul Spawned"))
							NewSoul->SetItemState(EItemState::EIS_Hovering);
						}
					}
				}
			}
		}
	}

	PlayHitSound(ImpactPoint);
	SpawnHitParticles(ImpactPoint);
	if (!IsDead())
	{
		ShowHealthBar();
	}
	ClearPatrolTimer();

}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	if (PawnSensing)
	{
		PawnSensing->OnSeePawn.AddDynamic(this, &AEnemy::PawnSeen);
	}

	InitializeEnemy();
	PawnSensing->SightRadius = 4000.f;
	Tags.Add(FName(TEXT("Enemy")));
}

void AEnemy::Die()
{
	Super::Die();

	EnemyState = EEnemyState::EES_Dead;
	ClearAttackTimer();
	HideHealthBar();
	DisableCapsule();
	SetLifeSpan(DeathLifeSpan);
	GetCharacterMovement()->bOrientRotationToMovement = false;
	SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AEnemy::Attack()
{
	Super::Attack();
	if(CombatTarget == nullptr)
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


	EnemyState = EEnemyState::EES_Engaged;
	PlayAttackMontage();
}

bool AEnemy::CanAttack()
{
	bool bCanAttack =
		IsInAttackRadius() &&
		!IsAttacking() &&
		!IsEngaged() &&
		!IsDead();

	return bCanAttack;
}

void AEnemy::HandleDamage(float DamageAmount)
{
	Super::HandleDamage(DamageAmount);

	if (Attributes && HealthBarWidget)
	{
		HealthBarWidget->SetHealthPercent(Attributes->GetHealthPercent());
	}
}

int32 AEnemy::PlayDeathMontage()
{
	const int32 Selection = Super::PlayDeathMontage();
	TEnumAsByte<EDeathPose> Pose(Selection);	// ������ enum���� ��ȯ���ִ� ���ø� Ŭ����, enum class���� ����ϴ� �� ���� ����, ���� EDeathPose�� �׳� enum���� �ٲ��������
	if (Pose < EDeathPose::EDP_MAX)
	{
		DeathPose = Pose;
	}
	return Selection;
}

void AEnemy::AttackEnd()
{
	GetCharacterMovement()->MaxWalkSpeed = ChasingSpeed;
	EnemyState = EEnemyState::EES_NoState;
	CheckCombatTarget();
}

void AEnemy::StopMove()
{
	if (!EnemyController)
		return;

	EnemyController->StopMovement();
}

bool AEnemy::InTargetRange(AActor* Target, double Radius)
{
	if (!Target) return false;

	const double DistanceToTarget = (Target->GetActorLocation() - GetActorLocation()).Size();
	return DistanceToTarget <= Radius;
}

void AEnemy::MoveToTarget(AActor* Target)
{
	if (!EnemyController || !Target)
		return;

	FAIMoveRequest MoveRequest;
	MoveRequest.SetGoalActor(Target);
	MoveRequest.SetAcceptanceRadius(50.f);

	EnemyController->MoveTo(MoveRequest);
}

AActor* AEnemy::ChoosePatrolTarget()
{
	TArray<AActor*> ValidTargets;
	for (auto Target : PatrolTargets)
	{
		if (Target != PatrolTarget)
		{
			ValidTargets.AddUnique(Target);
		}
	}

	const int32 NumPatrolTargets = ValidTargets.Num();
	if (NumPatrolTargets > 0)
	{
		const int32 TargetSelection = FMath::RandRange(0, NumPatrolTargets - 1);
		return ValidTargets[TargetSelection];
	}

	return nullptr;
}

void AEnemy::StartAttackTimer()
{
	EnemyState = EEnemyState::EES_Attacking;
	const float AttackTime = FMath::RandRange(AttackMin, AttackMax);
	GetWorldTimerManager().SetTimer(AttackTimer, this, &AEnemy::Attack, AttackTime);
}

void AEnemy::ClearAttackTimer()
{
	GetWorldTimerManager().ClearTimer(AttackTimer);
}

void AEnemy::HideHealthBar()
{
	if (HealthBarWidget)
	{
		HealthBarWidget->SetVisibility(false);
	}
}

void AEnemy::ShowHealthBar()
{
	if (HealthBarWidget)
	{
		HealthBarWidget->SetVisibility(true);
	}
}

void AEnemy::LoseInterest()
{
	CombatTarget = nullptr;
	HideHealthBar();
}

void AEnemy::StartPatrolling()
{
	EnemyState = EEnemyState::EES_Patrolling;
	GetCharacterMovement()->MaxWalkSpeed = PatrollingSpeed;
	MoveToTarget(PatrolTarget);
}

void AEnemy::ChaseTarget()
{
	EnemyState = EEnemyState::EES_Chasing;
	GetCharacterMovement()->MaxWalkSpeed = ChasingSpeed;
	MoveToTarget(CombatTarget);
}

bool AEnemy::IsOutsideCombatRadius()
{
	return !InTargetRange(CombatTarget, CombatRadius);
}

bool AEnemy::IsOutsideAttackRadius()
{
	return !InTargetRange(CombatTarget, AttackRadius);
}

bool AEnemy::IsInAttackRadius()
{
	return InTargetRange(CombatTarget, AttackRadius);
}

bool AEnemy::IsChasing()
{
	return EnemyState == EEnemyState::EES_Chasing;
}

bool AEnemy::IsAttacking()
{
	return EnemyState == EEnemyState::EES_Attacking;
}

bool AEnemy::IsDead()
{
	return EnemyState == EEnemyState::EES_Dead;
}

bool AEnemy::IsEngaged()
{
	return EnemyState == EEnemyState::EES_Engaged;
}

void AEnemy::ClearPatrolTimer()
{
	GetWorldTimerManager().ClearTimer(PatrolTimer);	// �߰��� �����ϱ� ������ Ÿ�̸� �ʱ�ȭ
}

void AEnemy::PatrolTimerFinished()
{
	MoveToTarget(PatrolTarget);
}

void AEnemy::CheckPatrolTarget()
{
	if (InTargetRange(PatrolTarget, PatrolRadius))
	{
		PatrolTarget = ChoosePatrolTarget();
		const float WaitTime = FMath::RandRange(PatrolWaitMin, PatrolWaitMax);
		// Ÿ�̸Ӹ� ������, �Ű������� Ÿ�̸� ����, �Լ��� �ִ� ��ü, Ÿ�̸Ӱ� �� �Ǿ��� �� ����� �Լ�, �ð��� ����
		GetWorldTimerManager().SetTimer(PatrolTimer, this, &AEnemy::PatrolTimerFinished, WaitTime);
	}
}

void AEnemy::CheckCombatTarget()
{
	// �ٱ��� ClearAttackTimer�� �������� �ʴ� ������ ƽ �߿��� �� ���ǵ��� ���� �����ϴ� ��찡 ���� �ٵ� 
	// �׶��� ClearAttackTimer�� �����ؼ� ���ɿ� ������ ���� �ʰ� �ϱ� ���ؼ���
	if (IsOutsideCombatRadius())
	{
		// ���� Ÿ�̸� �ʱ�ȭ
		ClearAttackTimer();
		// ���� ���� ������ ������ ��׷ΰ� Ǯ������ ����
		LoseInterest();
		// ���� ����
		if (!IsEngaged())
		{
			StartPatrolling();
		}
	}

	else if (IsOutsideAttackRadius() && !IsChasing())
	{
		// ���� Ÿ�̸� �ʱ�ȭ
		ClearAttackTimer();
		// ���� ���� ���̸� ���ΰ��� ������
		if (!IsEngaged()) 
		{
			ChaseTarget();
		}
	}

	else if (CanAttack())
	{
		StartAttackTimer();
	}
}

void AEnemy::SpawnDefaultWeapon()
{
	UWorld* World = GetWorld();
	if (World && WeaponClass.Num() > 0)
	{
		for (int i = 0; i < WeaponClass.Num(); ++i)
		{
			if (WeaponClass[i] && i == 0)
			{
				AWeapon* DefaultWeapon = World->SpawnActor<AWeapon>(WeaponClass[i]);
				DefaultWeapon->Equip(GetMesh(), FName(TEXT("RightHandSocket")), this, this);
				EquippedWeapon[i] = DefaultWeapon;
			}
			else if (WeaponClass[i] && i == 1)
			{
				AWeapon* DefaultWeapon = World->SpawnActor<AWeapon>(WeaponClass[i]);
				DefaultWeapon->Equip(GetMesh(), FName(TEXT("LeftHandSocket")), this, this);
				EquippedWeapon[i] = DefaultWeapon;
			}
			else if (WeaponClass[i] && i == 2)
			{
				AWeapon* DefaultWeapon = World->SpawnActor<AWeapon>(WeaponClass[i]);
				DefaultWeapon->Equip(GetMesh(), FName(TEXT("HeadSocket")), this, this);
				EquippedWeapon[i] = DefaultWeapon;
			}
		}
	}
}

void AEnemy::InitializeEnemy()
{
	EnemyController = Cast<AAIController>(GetController());
	MoveToTarget(PatrolTarget);

	if (HealthBarWidget)
	{
		HealthBarWidget->SetHealthPercent(Attributes->GetHealthPercent());
	}

	HideHealthBar();
	SpawnDefaultWeapon();
}

void AEnemy::PawnSeen(APawn* SeenPawn)
{
	const bool bShouldChaseTarget =
		!IsChasing() &&
		!IsDead() &&
		EnemyState < EEnemyState::EES_Attacking &&
		SeenPawn->ActorHasTag(FName(TEXT("EngageableTarget")));

	if (bShouldChaseTarget)
	{
		CombatTarget = SeenPawn;
		ClearPatrolTimer();
		ChaseTarget();
	}
}

// AActor::TakeDamage
// float DamageAmount: ����� ��ġ
// FDamageEvent& DamageEvent: ������� ���� �߰����� ����
// AController* Instigator: ������� ���� ��Ʈ�ѷ�
// AActor* DamageCauser: ���������� ������� ���� ����

// UGameplayStatics::ApplyDamage
// DamageTyepClass: ����� Ÿ���� ���ؼ� ����ڰ� Ŀ�������� �����Ÿ�Կ� ���� ó���� �� �� �ֵ��� ��

// ���� �÷��̾ ������ ������� �����ٸ� UGameplayStatics::ApplyDamage()�� ȣ���ϸ鼭 �Ű������� ���� �ѱ�
// ������� ���� ���� ApplyDamage���� �Ѱܹ��� ������ �������� AActor::TakeDamage�� ȣ���Ͽ� ������� ���� ó���� ��



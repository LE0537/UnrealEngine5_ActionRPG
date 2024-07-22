// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Characters/CharacterTypes.h"
#include "Characters/BaseCharacter.h"
#include "Enemy.generated.h"

class UHealthBarComponent;
class UPawnSensingComponent;
class AAIController;
class AWeapon;

UCLASS()
class TEST2_API AEnemy : public ABaseCharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy();

	// Actor
	virtual void Tick(float DeltaTime) override;
	virtual void Destroyed() override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	// ~Actor

	// HitInterface
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;
	// HitInterface

	UPROPERTY(VisibleAnywhere)
	UPawnSensingComponent* PawnSensing;

protected:
	// Actor
	virtual void BeginPlay() override;
	// ~Actor

	// BaseCharacter
	virtual void Die() override;
	virtual void Attack() override;
	virtual bool CanAttack() override;
	virtual void HandleDamage(float DamageAmount) override;
	virtual int32 PlayDeathMontage() override;
	virtual void AttackEnd() override;
	// ~BaseCharacter
	void StopMove();
	bool InTargetRange(AActor* Target, double Radius);
	void MoveToTarget(AActor* Target);
	AActor* ChoosePatrolTarget();

	UPROPERTY(BlueprintReadOnly)
	EEnemyState EnemyState;
	/*
	*	������Ʈ
	*/

	UPROPERTY(VisibleAnywhere)
	UHealthBarComponent* HealthBarWidget;



	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<AWeapon>> WeaponClass;

	/*
	*	����
	*/

	UPROPERTY(EditAnywhere)
	float CombatRadius;

	UPROPERTY(EditAnywhere)
	float AttackRadius;

	void StartAttackTimer();
	void ClearAttackTimer();

	FTimerHandle AttackTimer;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float AttackMin = 0.5f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float AttackMax = 1.f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float PatrollingSpeed;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float ChasingSpeed;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float DeathLifeSpan = 8.f;

	/*
	*	AI �׺���̼�
	*/

	UPROPERTY()
	AAIController* EnemyController;

	// ���� ���� ��ǥ
	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	AActor* PatrolTarget;

	// ���� ��ǥ ����
	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	TArray<AActor*> PatrolTargets;

	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	double PatrolRadius;

	FTimerHandle PatrolTimer;

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float PatrolWaitMin = 5.f;							// Ctrl + R -> Ctrl + R ������ �ַ�� ���� �ִ� �ش� �̸��� �������� �� ���� �� �ٲ� �� ����

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float PatrolWaitMax = 10.f;

	void HideHealthBar();
	void ShowHealthBar();
	void LoseInterest();
	void StartPatrolling();
	void ChaseTarget();
	bool IsOutsideCombatRadius();
	bool IsOutsideAttackRadius();
	bool IsInAttackRadius();
	bool IsChasing();
	bool IsAttacking();
	bool IsDead();
	bool IsEngaged();
	void ClearPatrolTimer();
	void PatrolTimerFinished();
	void CheckPatrolTarget();
	void CheckCombatTarget();
	void SpawnDefaultWeapon();
	void InitializeEnemy();

	UFUNCTION()
	void PawnSeen(APawn* SeenPawn);		// �� ���� ������Ʈ�� OnPawnSeen(���� �������� ��) �̺�Ʈ�� ���� �ݹ� �Լ�


};

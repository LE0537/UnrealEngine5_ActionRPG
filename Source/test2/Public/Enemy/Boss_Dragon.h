// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/Enemy.h"
#include "Boss_Dragon.generated.h"


class UBossHUD;

UCLASS()
class TEST2_API ABoss_Dragon : public AEnemy
{
	GENERATED_BODY()

public:
	ABoss_Dragon();
	virtual void Tick(float DeltaTime) override;

	virtual void Destroyed() override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;

protected:
	virtual void BeginPlay() override;

	virtual void Die() override;
	virtual void Attack() override;
	virtual bool CanAttack() override;
	virtual void HandleDamage(float DamageAmount) override;
	virtual int32 PlayDeathMontage() override;
	virtual void AttackEnd() override;

	UFUNCTION(BlueprintCallable)
	void SetGroundSkill1(ECollisionEnabled::Type CollisionEnabled);


	UFUNCTION(BluePrintCallable)
	void SpawnCutSceneEnd();

	UFUNCTION(BluePrintCallable)
	void SpawnStartFail();

	UPROPERTY(EditAnywhere)
	TSubclassOf< class ADragonGroundSkill1> GroundSkill1Class;

	UPROPERTY(VisibleAnywhere, Category = "Skill")
	ADragonGroundSkill1* GroundSkill1;
private:
	void ChaseCombatTarget();
	void StartSkill1Timer();
	void ClearSkill1Timer();
	void StartSkill2Timer();
	void ClearSkill2Timer();
	void StartSkill3Timer();
	void ClearSkill3Timer();
	void StartSkill4Timer();
	void ClearSkill4Timer();


	void Boss_Skill1();
	void Boss_Skill2();
	void Boss_Skill3();
	void Boss_Skill4();

	void ShowMesh();

	void StartFly(float _duration, float Value);

	UFUNCTION(BlueprintCallable)
	void EndFly(float _duration);

	void CheckGround();
	void AttachMeshToSocket(const FName& InSocketName);

	UFUNCTION(BlueprintCallable, Category = "FireBall")
	void LaunchFireBall();

	UPROPERTY(EditAnywhere, Category = "FireBall")
	TSubclassOf<class AFireBall> FireBallClass;

	UFUNCTION(BlueprintCallable, Category = "FireStorm")
	void SpawningFireStorm();

	UPROPERTY(EditAnywhere, Category = "FireStorm")
	TSubclassOf<class AFireStorm> FireStormClass;

	void PlaySkillMontage(UAnimMontage* Montage, const FName& SectionName);
	void PlaySpawnMontage();

	FTimerHandle Skill1Timer;
	FTimerHandle Skill2Timer;
	FTimerHandle Skill3Timer;
	FTimerHandle Skill4Timer;

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* SkillMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* Skill2Montage;

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* Skill3Montage;

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* Skill4Montage;

	UPROPERTY(EditAnywhere, Category = "Combat")
	TArray<FName>SkillMontageSections;

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* SpawnMontage;

	UPROPERTY(EditAnywhere, Category = "Combat")
	FName SpawnMontageSections;

	UPROPERTY(EditDefaultsOnly, Category = "HUD")
	TSubclassOf<UBossHUD> BossHUDClass;
	UPROPERTY()
	UBossHUD* BossHUD;

	bool bSkillStart[4] = { false, false, false, false };
	bool bSkillReserve[4] = { false, false,false, false };
	bool SpawnCutScene = true;
	
	int32 Steps;
	int32 EndSteps;
	float CurrentValue = 5000.f;
	float GroudZ = 401.f;
	FTimerHandle TimerHandle;
	float FlyTargetValue = 2000.f;
	float FlySkill2Value = 1000.f;
	bool bFlying = false;
	float FlyCoolTime = 0.f;

	const FVector FireStormSpawnLoc = { 10960.f, -16820.f, 100.f };
};

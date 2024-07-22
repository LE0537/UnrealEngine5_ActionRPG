// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/Enemy.h"
#include "Boss_Wolf.generated.h"

class UBossHUD;

UCLASS()
class TEST2_API ABoss_Wolf : public AEnemy
{
	GENERATED_BODY()
	
public :
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

	UFUNCTION(BluePrintCallable)
	void SpawnPoisonBall();

	UFUNCTION(BluePrintCallable)
	void SpawnCutSceneEnd();
	UFUNCTION(BluePrintCallable)
	void SpawnCutSceneShowMesh();

	UPROPERTY(EditDefaultsOnly, Category = "HUD")
	TSubclassOf<UBossHUD> BossHUDClass;
	UPROPERTY()
	UBossHUD* BossHUD;
private:
	void ChaseCombatTarget();
	void StartSkill1Timer();
	void ClearSkill1Timer();
	void StartSkill2Timer();
	void ClearSkill2Timer();

	void Boss_Skill1();
	void Boss_Skill2();

	void ShowMesh();


	UFUNCTION(BlueprintCallable, Category = "Poison")
	void LaunchPoison();

	UPROPERTY(EditAnywhere, Category = "Poison")
	TSubclassOf<class APoisonBall> PoisonBallClass;

	void PlaySkillMontage(UAnimMontage* Montage, const FName& SectionName);
	void PlaySpawnMontage();

	FTimerHandle Skill1Timer;
	FTimerHandle Skill2Timer;

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* SkillMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* Skill2Montage;

	UPROPERTY(EditAnywhere, Category = "Combat")
	TArray<FName>SkillMontageSections;

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* SpawnMontage;

	UPROPERTY(EditAnywhere, Category = "Combat")
	FName SpawnMontageSections;

	bool bSkillStart[2] = { false, false };
	bool bSkillReserve[2] = { false, false };
	bool SpawnCutScene = true;

};

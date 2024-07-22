// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Characters/CharacterTypes.h"
#include "SkillComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TEST2_API USkillComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USkillComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void PlaySkillMontage(const FName& SectionName);
	void PlaySkillMontage(int32 SectionIndex);
	void PlaySkillMontage(EActionState SectionState);
	void StopSkillMontage();
	float GetSkillCooldown(int32 SectionIndex);
	float GetSkillCooldown(EActionState SectionState);
	bool IsSkillAvailable(int32 SectionIndex);
	bool IsSkillAvailable(EActionState SectionState);
	bool IsSkillOneAvailable();
	int32 SectionStateToIndex(EActionState SectionState);

	float GetMaxSkillCooldown(int32 SectionIndex);
	bool IsValidIndex(int32 SectionIndex);
	void ResetCooldown(int32 SectionIndex);
	void ResetCooldown(EActionState SectionState);
	void RefillCooldown();
	void SetMaxSkillOneCharge(int32 Value);
	bool IsSkillOneChargeFull();
	void DecreaseSkillOneCharge();
	void IncreaseSkillOneCharge();
	void IncreaseSkillLevel(EActionState SectionState);
	int32 GetSkillLevel(EActionState SectionState);
	float GetSkillCooldownPercent(EActionState SectionState);
	float GetSkillOneChargeRegenCounter() { return SkillOneChargeRegenCounter; }
	void SetSkillOneChargeRegenCounter(float Value) { SkillOneChargeRegenCounter = Value; }
	float GetSkillOneChargeRegenTime() { return MaxSkillCooldown[0]; }
	int32 GetSkillOneCharge() { return RemainingSkillOneCharge; }

	void Spawn(const FVector& Location, const FRotator& Rotation, APawn* InstigatorInput);
	void Shoot();

	float SkillOneManaConsumption = 30.f;
	float SkillThreeManaConsumption = 60.f;
	float SkillFourManaConsumption = 100.f;


private:
	UPROPERTY(VisibleAnywhere, Category = "Skill")
	TArray<class AProjectile*> SpawnedProjectiles;

	UPROPERTY(EditDefaultsOnly, Category = "Montage")
	UAnimMontage* SkillMontage;

	TArray<float> SkillCooldown;

	UPROPERTY(VisibleAnywhere, Category = "Montage")
	TArray<float> MaxSkillCooldown;

	UPROPERTY(VisibleAnywhere, Category = "Montage")
	TArray<FName> SkillSectionName;

	TArray<int32> CurrentSkillLevel;
	
	int32 RemainingSkillOneCharge;
	int32 MaxSkillOneCharge;
	float SkillOneChargeRegenCounter;
};

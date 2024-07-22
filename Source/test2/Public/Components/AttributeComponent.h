// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttributeComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TEST2_API UAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAttributeComponent();
	void ReceiveDamage(float Damage);
	void UseMana(float ManaUsage);
	float GetHealthPercent();
	float GetManaPercent();
	bool IsAlive();
	void ResetDodgeCooldown();
	void ResetRollCooldown();
	void IncreaseDodgeCooldown(float Value);
	void IncreaseRollCooldown(float Value);
	void DecreaseDodgeCharge();
	void IncreaseDodgeChargeCooldown(float Value);
	void ResetDodgeChargeCooldown();
	int32 GetHPPotion() { return HPPotion; }
	int32 GetMPPotion() { return MPPotion; }
	void SetMPPotion(int32 Value) { MPPotion = Value; }
	void SetHPPotion(int32 Value) { HPPotion = Value; }
	float GetHPPotionHealingAmount() { return HPPotionHealingAmount; }
	float GetMPPotionHealingAmount() { return MPPotionHealingAmount; }
	void HealingHP(float Healing);
	void HealingMP(float Healing);
	void ManaRegenPerSecond();
	void ManaRegen();
	void AddSoul(int32 Value);
	void AddGold(int32 Value);
	FORCEINLINE int32 GetGold() { return Gold; }
	FORCEINLINE int32 GetSoul() { return Soul; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool GetDodgeUpgrade() { return bDodgeUpgrade; }
	FORCEINLINE bool IsDodgeCooldownOver() { return DodgeCooldown <= 0.f; }
	FORCEINLINE bool IsRollCooldownOver() { return RollCooldown <= 0.f; }
	FORCEINLINE bool IsDodgeChargeZero() { return DodgeCharge <= 0; }
	FORCEINLINE bool IsDodgeChargeFull() { return DodgeCharge >= MaxDodgeCharge; }
	FORCEINLINE bool IsDodgeChargeCooldownOver() { return DodgeChargeCooldown <= 0.f; }
	FORCEINLINE bool IsSufficientMana(float Usage) { return Usage <= Mana; }
	FORCEINLINE float GetDodgeCooldown() { return DodgeCooldown; }
	FORCEINLINE float GetDodgeCooldownPercent() { return DodgeCooldown / DodgeCooldownTime; }
	FORCEINLINE float GetDodgeChargeCooldownPercent() { return DodgeChargeCooldown / DodgeChargeTime; }
	FORCEINLINE float GetDodgeChargeCooldown() { return DodgeChargeCooldown; }
	FORCEINLINE float GetDodgeCharge() { return DodgeCharge; }
	FORCEINLINE float GetRollCooldownPercent() { return RollCooldown / RollCooldownTime; }
	FORCEINLINE float GetRollCooldown() { return RollCooldown; }
	FORCEINLINE float GetHP() { return Health; }
	FORCEINLINE float GetMaxHP() { return MaxHealth; }
	FORCEINLINE float GetMP() { return Mana; }
	FORCEINLINE float GetMaxMP() { return MaxMana; }
	
	UFUNCTION()
	void IncreaseDodgeCharge();

	void SetDodgeUpgrade(bool Value);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	// ���� ü��
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float Health;
	
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float MaxHealth;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float Mana;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float MaxMana;

	UPROPERTY(VisibleAnywhere)
	int8 DodgeCharge;

	UPROPERTY(EditAnywhere)
	int8 MaxDodgeCharge;

	UPROPERTY(VisibleAnywhere)
	float DodgeChargeCooldown;

	UPROPERTY(EditAnywhere)
	float DodgeChargeTime;

	UPROPERTY(VisibleAnywhere)
	float DodgeCooldown;

	UPROPERTY(EditAnywhere)
	float DodgeCooldownTime;

	UPROPERTY(VisibleAnywhere)
	float RollCooldown;

	UPROPERTY(EditAnywhere)
	float RollCooldownTime;

	UPROPERTY(EditAnywhere)
	bool bDodgeUpgrade;

	UPROPERTY(EditAnywhere)
	int32 Gold;

	UPROPERTY(EditAnywhere)
	int32 Soul;

	UPROPERTY(EditAnywhere)
	int32 HPPotion;

	UPROPERTY(EditAnywhere)
	int32 MPPotion;

	UPROPERTY(EditAnywhere)
	float HPPotionHealingAmount;

	UPROPERTY(EditAnywhere)
	float MPPotionHealingAmount;

	FTimerHandle ManaRegenTimer;
};

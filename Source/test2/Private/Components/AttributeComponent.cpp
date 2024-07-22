// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/AttributeComponent.h"

// Sets default values for this component's properties
UAttributeComponent::UAttributeComponent() :
	Health(100.f),
	MaxHealth(100.f),
	Mana(200.f),
	MaxMana(200.f),
	MaxDodgeCharge(3),
	DodgeCharge(3),
	DodgeChargeCooldown(0.f),
	DodgeCooldown(0.f),
	DodgeCooldownTime(2.f),
	RollCooldown(0.f),
	RollCooldownTime(4.f), 
	DodgeChargeTime(2.f),
	bDodgeUpgrade(false),
	HPPotion(3), 
	MPPotion(3),
	HPPotionHealingAmount(100.f),
	MPPotionHealingAmount(40.f)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void UAttributeComponent::ReceiveDamage(float Damage)
{
	Health = FMath::Clamp(Health - Damage, 0.f, MaxHealth);	// Clamp, ���� �ִ񰪰� �ּڰ� ���̷� ����ִ� �Լ�, 
	// �ִ񰪺��� ū ���� ������ �ִ��� ��ȯ�ϰ�, �ּڰ����� ���� ���� ������ �ּڰ��� ��ȯ
}

void UAttributeComponent::UseMana(float ManaUsage)
{
	Mana = FMath::Clamp(Mana - ManaUsage, 0.f, MaxMana);
}

float UAttributeComponent::GetHealthPercent()
{
	return Health / MaxHealth;
}

float UAttributeComponent::GetManaPercent()
{
	return Mana / MaxMana;
}

bool UAttributeComponent::IsAlive()
{
	return Health > 0.f;
}

void UAttributeComponent::ResetDodgeCooldown()
{
	DodgeCooldown = DodgeCooldownTime;
}

void UAttributeComponent::ResetRollCooldown()
{
	RollCooldown = RollCooldownTime;
}

void UAttributeComponent::IncreaseDodgeCooldown(float Value)
{
	DodgeCooldown -= Value;
}

void UAttributeComponent::IncreaseRollCooldown(float Value)
{
	RollCooldown -= Value;
}

void UAttributeComponent::DecreaseDodgeCharge()
{
	DodgeCharge--;
}

void UAttributeComponent::IncreaseDodgeChargeCooldown(float Value)
{
	DodgeChargeCooldown -= Value;
}

void UAttributeComponent::ResetDodgeChargeCooldown()
{
	DodgeChargeCooldown = DodgeChargeTime;
}

void UAttributeComponent::HealingHP(float Healing)
{
	Health = FMath::Clamp(Health + Healing, 0.f, MaxHealth);
}

void UAttributeComponent::HealingMP(float Healing)
{
	Mana = FMath::Clamp(Mana + Healing, 0.f, MaxMana);
}

void UAttributeComponent::ManaRegenPerSecond()
{
	UWorld* World = GetWorld();
	if (World)
	{
		World->GetTimerManager().SetTimer(ManaRegenTimer, this, &UAttributeComponent::ManaRegen, 1.f, true);
	}
}

void UAttributeComponent::ManaRegen()
{
	HealingMP(3.f);
}

void UAttributeComponent::AddSoul(int32 Value)
{
	Soul += Value;
}

void UAttributeComponent::AddGold(int32 Value)
{
	Gold += Value;
}

void UAttributeComponent::IncreaseDodgeCharge()
{
	if (DodgeCharge >= MaxDodgeCharge)
	{
		return;
	}

	DodgeCharge++;
}

void UAttributeComponent::SetDodgeUpgrade(bool Value)
{
	bDodgeUpgrade = Value;
}


// Called when the game starts
void UAttributeComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
}


// Called every frame
void UAttributeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
	if (MaxDodgeCharge > DodgeCharge)
	{
		IncreaseDodgeCooldown(DeltaTime);
		IncreaseDodgeChargeCooldown(DeltaTime);

		if (IsDodgeChargeCooldownOver())
		{
			ResetDodgeChargeCooldown();
			IncreaseDodgeCharge();
		}
	}
	
	IncreaseRollCooldown(DeltaTime);
}


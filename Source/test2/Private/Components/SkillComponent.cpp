// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/SkillComponent.h"
#include "GameFramework/Character.h"
#include "PlayerProjectile/Projectile.h"
#include "UObject/UObjectGlobals.h"
#include "Components/BoxComponent.h"

// Sets default values for this component's properties
USkillComponent::USkillComponent() : RemainingSkillOneCharge(3), MaxSkillOneCharge(3)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void USkillComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	for (int32 i = 0; i < 4; ++i)
	{
		SkillSectionName.Add(SkillMontage->GetSectionName(i));
		SkillCooldown.Add(0.f);
		MaxSkillCooldown.Add(20.f);
		CurrentSkillLevel.Add(1);
	}
	MaxSkillCooldown[0] = 10.f;
	MaxSkillCooldown[2] = 30.f;
	MaxSkillCooldown[3] = 60.f;
}

void USkillComponent::PlaySkillMontage(const FName& SectionName)
{
	ACharacter* Owner = Cast<ACharacter>(GetOwner());
	USkeletalMeshComponent* OwnerMesh = Owner->GetMesh();
	UAnimInstance* AnimInstance = OwnerMesh->GetAnimInstance();
	if (AnimInstance && SkillMontage)
	{
		AnimInstance->Montage_Play(SkillMontage);
		AnimInstance->Montage_JumpToSection(SectionName, SkillMontage);
	}
}

void USkillComponent::PlaySkillMontage(int32 SectionIndex)
{
	FName SectionName = SkillMontage->GetSectionName(SectionIndex);
	PlaySkillMontage(SectionName);
}

void USkillComponent::PlaySkillMontage(EActionState SectionState)
{
	int32 SectionIndex = SectionStateToIndex(SectionState);
	PlaySkillMontage(SectionIndex);
}

void USkillComponent::StopSkillMontage()
{
	ACharacter* Owner = Cast<ACharacter>(GetOwner());
	USkeletalMeshComponent* OwnerMesh = Owner->GetMesh();
	UAnimInstance* AnimInstance = OwnerMesh->GetAnimInstance();
	if (AnimInstance && SkillMontage)
	{
		AnimInstance->Montage_Stop(0.25f, SkillMontage);
	}
}

float USkillComponent::GetSkillCooldown(int32 SectionIndex)
{
	if (SkillCooldown.IsValidIndex(SectionIndex))
	{
		return SkillCooldown[SectionIndex];
	}

	else return INFINITY;
}

float USkillComponent::GetSkillCooldown(EActionState SectionState)
{
	int32 SectionIndex = SectionStateToIndex(SectionState);
	return GetSkillCooldown(SectionIndex);
}

bool USkillComponent::IsSkillAvailable(int32 SectionIndex)
{
	if (!IsValidIndex(SectionIndex))
	{
		return false;
	}

	if (SkillCooldown[SectionIndex] <= 0.f)
	{
		return true;
	}

	else
	{
		return false;
	}
}

bool USkillComponent::IsSkillAvailable(EActionState SectionState)
{
	int32 SectionIndex = SectionStateToIndex(SectionState);
	return IsSkillAvailable(SectionIndex);
}

bool USkillComponent::IsSkillOneAvailable()
{
	if (RemainingSkillOneCharge > 0)
	{
		return true;
	}

	else
	{
		return false;
	}
}

int32 USkillComponent::SectionStateToIndex(EActionState SectionState)
{
	int32 ret = static_cast<int32>(SectionState) - static_cast<int32>(EActionState::EAS_Skill_1);
	if (IsValidIndex(ret))
	{
		return ret;
	}

	else return INFINITY;
}

float USkillComponent::GetMaxSkillCooldown(int32 SectionIndex)
{
	if (SkillCooldown.IsValidIndex(SectionIndex))
	{
		return MaxSkillCooldown[SectionIndex];
	}

	else return INFINITY;
}

bool USkillComponent::IsValidIndex(int32 SectionIndex)
{
	if (SkillCooldown.IsValidIndex(SectionIndex) &&
		MaxSkillCooldown.IsValidIndex(SectionIndex))
		return true;

	else 
		return false;
}

void USkillComponent::ResetCooldown(int32 SectionIndex)
{
	if (IsValidIndex(SectionIndex))
	{
		SkillCooldown[SectionIndex] = MaxSkillCooldown[SectionIndex];
	}
}

void USkillComponent::ResetCooldown(EActionState SectionState)
{
	int32 SectionIndex = SectionStateToIndex(SectionState);
	ResetCooldown(SectionIndex);
}

void USkillComponent::RefillCooldown()
{
	for (auto& Cooldown : SkillCooldown)
	{
		Cooldown = 0.f;
	}
}

void USkillComponent::SetMaxSkillOneCharge(int32 Value)
{
	MaxSkillOneCharge = Value;
}

bool USkillComponent::IsSkillOneChargeFull()
{
	return RemainingSkillOneCharge == MaxSkillOneCharge;
}

void USkillComponent::DecreaseSkillOneCharge()
{
	RemainingSkillOneCharge = FMath::Clamp(--RemainingSkillOneCharge, 0, MaxSkillOneCharge); 
}

void USkillComponent::IncreaseSkillOneCharge()
{
	RemainingSkillOneCharge = FMath::Clamp(++RemainingSkillOneCharge, 0, MaxSkillOneCharge);
}

void USkillComponent::IncreaseSkillLevel(EActionState SectionState)
{
	int32 SectionIndex = SectionStateToIndex(SectionState);
	++CurrentSkillLevel[SectionIndex];
}

int32 USkillComponent::GetSkillLevel(EActionState SectionState)
{
	int32 SectionIndex = SectionStateToIndex(SectionState);
	return CurrentSkillLevel[SectionIndex];
}

float USkillComponent::GetSkillCooldownPercent(EActionState SectionState)
{
	int32 SectionIndex = SectionStateToIndex(SectionState);
	return SkillCooldown[SectionIndex] / MaxSkillCooldown[SectionIndex];
}

void USkillComponent::Spawn(const FVector& Location, const FRotator& Rotation, APawn* InstigatorInput)
{
	UWorld* World = GetWorld();

	if (World)
	{
		
		FString BlueprintPath = TEXT("/Game/Blueprints/Character/BP_PlayerProjectile.BP_PlayerProjectile_C");
		UClass* BlueprintClass = Cast<UClass>(StaticLoadObject(UClass::StaticClass(), nullptr, *FName(*BlueprintPath).ToString()));

		if (BlueprintClass)
		{
			FActorSpawnParameters ActorSpawnParameters;
			ActorSpawnParameters.Instigator = InstigatorInput;			

			// ���忡�� ���͸� �����մϴ�.
			AProjectile* NewProjectile = World->SpawnActor<AProjectile>(BlueprintClass, Location, Rotation, ActorSpawnParameters);
			if (NewProjectile)
			{
				NewProjectile->SetOwner(GetOwner());
				NewProjectile->SetItemState(EItemState::EIS_Equipped);
				SpawnedProjectiles.Add(NewProjectile);
			}
		}
	}
}

void USkillComponent::Shoot()
{
	if (SpawnedProjectiles.Num() > 0)
	{
		AProjectile* LaunchingProjectile = SpawnedProjectiles.Pop();
		LaunchingProjectile->Shoot();
	}
}


// Called every frame
void USkillComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...



	for(int32 i = 0; i < SkillCooldown.Num(); ++i)
	{
		if (i == 0 && IsSkillOneChargeFull())
		{
			continue;
		}
		else if (SkillCooldown[i] > 0.f)
		{
			SkillCooldown[i] -= DeltaTime;
		}
	}
}


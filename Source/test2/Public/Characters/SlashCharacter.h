// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseCharacter.h"
#include "CharacterTypes.h"
#include "Interfaces/PickupInterface.h"
#include "SlashCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class UGroomComponent;
class UAnimMontage;
class AItem;
class ASoul;
class USlashOverlay;
class USkillComponent;
class UBoxComponent;
class UNiagaraComponent;
class UInteractMessage;
class ACameraActor;
class ULevelSequence;
class ABGMManager;
UCLASS()
class TEST2_API ASlashCharacter : public ABaseCharacter, public IPickupInterface
{
	GENERATED_BODY()

public:
	ASlashCharacter();
	// FORCEINLINE: ������ �Լ��� �ζ������� ����� ��ũ��
	FORCEINLINE ECharacterState GetCharacterState() const { return CharacterState; }
	FORCEINLINE EActionState GetActionState() const { return ActionState; }
	FORCEINLINE void SetCutScene(bool CutScene) { isCutScene = CutScene; }
	FORCEINLINE void SetBGMManager(ABGMManager* _bgmmanager) { BGMManager = _bgmmanager; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE ABGMManager* GetBGMManager() { return BGMManager; }

	virtual void Tick(float DeltaTime) override;
	void UpdateSlashOverlay(float DeltaTime);
	void AdjustFOVWithSpeed(float DeltaTime);
	void SkillThreeBuffOff();
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;
	virtual void SetOverlappingItem(AItem* Item) override;
	virtual void AddSouls(ASoul* Soul) override;
	virtual void AddGold(float Gold) override;
	virtual void Jump() override;
	
	void TurnOnPickupUI();
	void TurnOffPickupUI();
	bool UpdateQuest(const FString& QuestName, int32 QuestUpdatedAmount = 1);
	void AddQuest(const FString& QuestName, const FString& QuestEntryString, int32 QuestObjectiveAmount = 1, int32 QuestCurrentAmount = 0, bool bIsDisplayingNumbers = false);
	bool IsQuestActive(const FString& QuestName);
	int32 GetQuestCurrentProgress(const FString& QuestName);

protected:
	virtual void BeginPlay() override;
	
	// �Է� �ݹ� �Լ���
	void MoveForward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);
	void LookUp(float Value);
	void EKeyPressed();
	void Equip();
	virtual void Attack() override;
	virtual void Die() override;
	void Dodge();
	void Roll();
	void ResetRollCooldown();
	void TestKey();

	// ���� ���� �Լ���
	void EquipWeapon(AWeapon* Weapon);
	virtual void AttackEnd() override;	// ������ ������ ������ �÷��̾��� ActionState�� EAS_Unoccupied�� �ٽ� �ǵ����� �ݹ� �Լ�
	void AttackComboReset();
	virtual bool CanAttack() override;
	bool CanDisarm();
	bool CanArm();
	void Disarm();
	void Arm();
	bool IsAttacking();
	bool IsAttackMontageIndexOverflowed();

	// ��ų ���� �Լ���
	void UseSkillOne();
	void UseSkillTwo();
	void UseSkillThree();
	void UseSkillFour();

	// ��Ÿ�� ���� �Լ���
	void PlayEquipMontage(const FName& SectionName);	// ���� ����ֱ�/������ ��Ÿ�� ��� �Լ�
	void PlayDodgeMontage(const FName& SectionName);
	virtual int32 PlayAttackMontage() override;

	UFUNCTION(BlueprintCallable)
	void AttachWeaponToBack();	// ���� ����ֱ� �Լ�

	UFUNCTION(BlueprintCallable)
	void AttachWeaponToHand();		// ���� ������ �Լ�

	UFUNCTION(BlueprintCallable)
	void FinishEquipping();	// ���� ����ֱ�/�����Ⱑ ������ ������ �÷��̾��� ActionState�� EAS_Unoccupied�� �ٽ� �ǵ����� �ݹ� �Լ�

	UFUNCTION(BlueprintCallable)
	void DodgeEnd();

	UFUNCTION(BlueprintCallable)
	void RollEnd();

	UFUNCTION(BlueprintCallable)
	void JumpEnd();

	UFUNCTION(BlueprintCallable)
	void HitReactEnd();

	UFUNCTION(BlueprintCallable)
	void AttackChainEnd();

	UFUNCTION(BlueprintCallable)
	void SkillEnd();

	UFUNCTION(BlueprintCallable)
	void SetSkillCollisionEnabled(ECollisionEnabled::Type CollisionEnabled);

	UFUNCTION(BlueprintCallable)
	bool IsSkillActive(int32 SkillIndex);

	UFUNCTION(BlueprintCallable)
	int32 GetCurrentSkillLevel(EActionState ActionState);

	UFUNCTION(BlueprintCallable)
	void Skill4Shoot();

	UFUNCTION(BlueprintCallable)
	void Skill4Spawn();

	UPROPERTY(EditDefaultsOnly)
	TArray<FName> Skill4SocketNameReady;

	UPROPERTY(VisibleAnywhere)
	TArray<FName> Skill4SocketNameFired;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UInteractMessage> InteractMessageClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	UInteractMessage* InteractMessage;

private:
	void InitializeSlashOverlay();
	void SetHUDHealth();
	void SetHUDMana();
	bool IsUnoccupied();
	void AttackCombo();
	void RefillCooldown();
	void DrinkHPPotion();
	void DrinkMPPotion();

	// ������Ʈ
	UPROPERTY(VisibleAnywhere)
	UCameraComponent* ViewCamera;

	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* SpringArm;

	UPROPERTY(EditAnywhere, Category = "Hair")
	UGroomComponent* Hair;

	UPROPERTY(EditAnywhere, Category = "Hair")
	UGroomComponent* Eyebrows;

	UPROPERTY(EditAnywhere)
	USkillComponent* Skill;

	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	TSubclassOf<AWeapon> SkillOneHitboxClass;

	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	AWeapon* SkillOneHitbox;

	UPROPERTY(VisibleInstanceOnly)
	AItem* OverlappingItem;

	ECharacterState CharacterState;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	EActionState ActionState;

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* EquipMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* DodgeMontage;

	UPROPERTY()
	USlashOverlay* SlashOverlay;

	UPROPERTY(EditDefaultsOnly, Category = "Skill")
	UNiagaraComponent* SkillThreeEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	USoundBase* PotionSound;

	UPROPERTY()
	int32 AttackMontageIndex;
	bool bIsAttackMontageIndexIncreased;
	bool bIsAttacked;
	bool bAttackable;
	bool bAttackCount[2] = {false, false};
	bool bAttacking[2] = { false, false };
	bool bSkillInvincibility;
	bool bIsSkillThreeActive;

	float DefaultFov = 90.f;
	FVector PreviousMeshPos;

	TArray<AActor*> ActorsToIgnore;

	float SkillThreeEffectTime = 15.f;
	float SkillThreeRemainingTime = 15.f;
	float DefenseBuffMultiplier = 1.f;

	float CameraDefaultHeight;
	float CameraSkill4Height;

	bool isCutScene = false;

	UPROPERTY()
	ABGMManager* BGMManager;
	bool bTest = false;
};

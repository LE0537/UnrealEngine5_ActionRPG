// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/SlashCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/AttributeComponent.h"
#include "GroomComponent.h"
#include "Items/Item.h"
#include "Items/Weapon/EnemyWeapon.h"
#include "Animation/AnimMontage.h"
#include "HUD/SlashHUD.h"
#include "HUD/SlashOverlay.h"
#include "Components/SkillComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"
#include "NiagaraComponent.h"
#include "HUD/InteractMessage.h"
#include "Items/Soul.h"
#include "BGMManager/BGMManager.h"


ASlashCharacter::ASlashCharacter() :
	CharacterState(ECharacterState::ECS_Unequipped),
	ActionState(EActionState::EAS_Unoccupied),
	AttackMontageIndex(0),
	bIsAttackMontageIndexIncreased(false),
	bIsAttacked(false),
	bAttackable(true),
	bSkillInvincibility(false)
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationYaw = false;		// ?뚮젅?댁뼱 紐⑤뜽??Yaw 諛⑺뼢?쇰줈 ?뚯쟾?섎뒗 寃껋쓣 寃곗젙?섎뒗 蹂??
	bUseControllerRotationPitch = false;	// ?뚮젅?댁뼱 紐⑤뜽??Pitch 諛⑺뼢?쇰줈 ?뚯쟾?섎뒗 寃껋쓣 寃곗젙?섎뒗 蹂??
	bUseControllerRotationRoll = false;		// ?뚮젅?댁뼱 紐⑤뜽??Roll 諛⑺뼢?쇰줈 ?뚯쟾?섎뒗 寃껋쓣 寃곗젙?섎뒗 蹂??

	UCharacterMovementComponent* Movement = GetCharacterMovement();
	Movement->bOrientRotationToMovement = true;
	Movement->RotationRate = FRotator(0.f, 400.f, 0.f);

	// 硫붿돩 肄쒕씪?대뜑 ?ㅼ젙
	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
	GetMesh()->SetGenerateOverlapEvents(true);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(GetRootComponent());
	SpringArm->TargetArmLength = 300.f;
	SpringArm->bUsePawnControlRotation = true;	// ?곗쓽 而⑦듃濡ㅻ윭???뚯쟾???ъ슜?섏뿬 ?ㅽ봽留곸븫???뚯쟾?쒗궗吏 寃곗젙?섎뒗 蹂??

	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));
	ViewCamera->SetupAttachment(SpringArm);
	DefaultFov = ViewCamera->FieldOfView;

	Hair = CreateDefaultSubobject<UGroomComponent>(TEXT("Hair"));
	Hair->SetupAttachment(GetMesh(), FName(TEXT("head")));		// 踰꾧렇?몄? 萸붿? ?묐룞???덈뤌???뚯폆 媛뺤젣吏??
	Hair->AttachmentName = FString("head");

	Eyebrows = CreateDefaultSubobject<UGroomComponent>(TEXT("Eyebrows"));
	Eyebrows->SetupAttachment(GetMesh(), FName(TEXT("head")));	// 踰꾧렇?몄? 萸붿? ?묐룞???덈뤌???뚯폆 媛뺤젣吏??
	Eyebrows->AttachmentName = FString("head");

	Skill = CreateDefaultSubobject<USkillComponent>(TEXT("Skill"));
	SkillThreeEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("SkillThreeEffect"));
	SkillThreeEffect->SetupAttachment(GetMesh());
	SkillThreeEffect->bAutoActivate = false;
}

void ASlashCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!bTest)
	{
		bTest = true;
		BGMManager->PlayBGM(EBGM::EBGM_Base);
	}

	if ((ActionState == EActionState::EAS_Attack_2 && !bAttacking[0]) ||
		(ActionState == EActionState::EAS_Attack_3 && !bAttacking[1]))
	{
		AttackCombo();
	}

	if (ActionState == EActionState::EAS_Skill_1 && !bSkillInvincibility)
	{
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Ignore);
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
		bSkillInvincibility = true;
	}

	AdjustFOVWithSpeed(DeltaTime);

	UpdateSlashOverlay(DeltaTime);

	if (bIsSkillThreeActive && SkillThreeRemainingTime <= 0.f)
	{
		SkillThreeBuffOff();
	}

	if (Skill->GetSkillCooldown(EActionState::EAS_Attack_1) <= 0.f && !Skill->IsSkillOneChargeFull())
	{
		Skill->IncreaseSkillOneCharge();
		SlashOverlay->SetSkillOneChargeCounter(Skill->GetSkillOneCharge());
		Skill->ResetCooldown(EActionState::EAS_Attack_1);
	}
	else if (Skill->IsSkillOneChargeFull())
	{
		SlashOverlay->SetSkillOneCooldownCounter(0.f);
		SlashOverlay->SetSkillOneBarPercent(0.f);
	}

	if (Attributes->IsDodgeChargeFull())
	{
		SlashOverlay->SetDodgeCooldownCounter(0.f);
		SlashOverlay->SetDodgeBarPercent(0.f);
	}

	SetHUDMana();
}

void ASlashCharacter::UpdateSlashOverlay(float DeltaTime)
{
	SlashOverlay->SetSkillOneBarPercent(Skill->GetSkillCooldownPercent(EActionState::EAS_Skill_1));
	SlashOverlay->SetSkillThreeBarPercent(Skill->GetSkillCooldownPercent(EActionState::EAS_Skill_3));
	SlashOverlay->SetSkillFourBarPercent(Skill->GetSkillCooldownPercent(EActionState::EAS_Skill_4));
	SlashOverlay->SetDodgeBarPercent(Attributes->GetDodgeChargeCooldownPercent());
	SlashOverlay->SetRollBarPercent(Attributes->GetRollCooldownPercent());

	SlashOverlay->SetSkillOneCooldownCounter(Skill->GetSkillCooldown(EActionState::EAS_Skill_1));
	SlashOverlay->SetSkillThreeCooldownCounter(Skill->GetSkillCooldown(EActionState::EAS_Skill_3));
	SlashOverlay->SetSkillFourCooldownCounter(Skill->GetSkillCooldown(EActionState::EAS_Skill_4));
	SlashOverlay->SetDodgeCooldownCounter(Attributes->GetDodgeChargeCooldown());
	SlashOverlay->SetDodgeChargeCounter(Attributes->GetDodgeCharge());
	SlashOverlay->SetRollCooldownCounter(Attributes->GetRollCooldown());

	if (bIsSkillThreeActive)
	{
		SkillThreeRemainingTime -= DeltaTime;
		SlashOverlay->SetSkillThreeActiveCounter(SkillThreeRemainingTime);
	}
}

void ASlashCharacter::AdjustFOVWithSpeed(float DeltaTime)
{
	if (ActionState == EActionState::EAS_Skill_1 || ActionState == EActionState::EAS_Dodge || ActionState == EActionState::EAS_Roll)
	{
		FVector CurrentMeshPos = GetMesh()->GetComponentLocation();
		float Speed = (DeltaTime > 0.f) ? (CurrentMeshPos - PreviousMeshPos).Size() / DeltaTime : 0.f;

		float TargetFov = DefaultFov + Speed * 0.03f;
		float CurrentFov = ViewCamera->FieldOfView;
		float NextFov = FMathf::Lerp(CurrentFov, TargetFov, 0.0375f);

		ViewCamera->SetFieldOfView(NextFov);
		PreviousMeshPos = CurrentMeshPos;
	}

	else if (ActionState == EActionState::EAS_Skill_4)
	{
		float TargetFov = DefaultFov + 15.f;
		float CurrentFov = ViewCamera->FieldOfView;
		float NextFov = FMathf::Lerp(CurrentFov, TargetFov, 0.0375f);

		ViewCamera->SetFieldOfView(NextFov);

		FVector CameraRelativeLocation = ViewCamera->GetRelativeLocation();
		float CurrentHeight = CameraRelativeLocation.Z;
		float NextHeight = FMathf::Lerp(CurrentHeight, CameraSkill4Height, 0.0375f);
		ViewCamera->SetRelativeLocation(FVector(CameraRelativeLocation.X, CameraRelativeLocation.Y, NextHeight));
	}

	else if (!FMath::IsNearlyEqual(ViewCamera->FieldOfView, DefaultFov, 0.1f) || (!FMath::IsNearlyEqual(ViewCamera->GetRelativeLocation().Z, CameraDefaultHeight, 0.1f)))
	{
		float CurrentFov = ViewCamera->FieldOfView;
		float NextFov = FMathf::Lerp(CurrentFov, DefaultFov, 0.1f);
		ViewCamera->SetFieldOfView(NextFov);

		FVector CameraRelativeLocation = ViewCamera->GetRelativeLocation();
		float CurrentHeight = CameraRelativeLocation.Z;
		float NextHeight = FMathf::Lerp(CurrentHeight, CameraDefaultHeight, 0.1f);
		ViewCamera->SetRelativeLocation(FVector(CameraRelativeLocation.X, CameraRelativeLocation.Y, NextHeight));
	}
}

void ASlashCharacter::SkillThreeBuffOff()
{
	bIsSkillThreeActive = false;
	SkillThreeRemainingTime = 0.f;
	SkillThreeEffect->Deactivate();

	for (auto& Weapon : EquippedWeapon)
	{
		if (IsValid(Weapon))
		{
			Weapon->BuffOff();
		}
	}

	DefenseBuffMultiplier = 1.f;
	SlashOverlay->SetSkillThreeActiveIcon(false);
	SlashOverlay->SetSkillThreeActiveCounter(-1.f);
}

void ASlashCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(FName(TEXT("MoveForward")), this, &ASlashCharacter::MoveForward);
	PlayerInputComponent->BindAxis(FName(TEXT("Turn")), this, &ASlashCharacter::Turn);
	PlayerInputComponent->BindAxis(FName(TEXT("LookUp")), this, &ASlashCharacter::LookUp);
	PlayerInputComponent->BindAxis(FName(TEXT("MoveRight")), this, &ASlashCharacter::MoveRight);

	// ?≪뀡 留ㅽ븨 ?⑥닔, IE_Pressed???뚮??????묐룞?쒕떎????
	PlayerInputComponent->BindAction(FName(TEXT("Jump")), IE_Pressed, this, &ASlashCharacter::Jump);
	PlayerInputComponent->BindAction(FName(TEXT("Equip")), IE_Pressed, this, &ASlashCharacter::Equip);
	PlayerInputComponent->BindAction(FName(TEXT("Interact")), IE_Pressed, this, &ASlashCharacter::EKeyPressed);
	PlayerInputComponent->BindAction(FName(TEXT("Attack")), IE_Pressed, this, &ASlashCharacter::Attack);
	PlayerInputComponent->BindAction(FName(TEXT("StepDodge")), IE_Pressed, this, &ASlashCharacter::Dodge);
	PlayerInputComponent->BindAction(FName(TEXT("Roll")), IE_Pressed, this, &ASlashCharacter::Roll);
	PlayerInputComponent->BindAction(FName(TEXT("Debug_RefillCooldown")), IE_Pressed, this, &ASlashCharacter::RefillCooldown);
	PlayerInputComponent->BindAction(FName(TEXT("Skill_1")), IE_Pressed, this, &ASlashCharacter::UseSkillOne);
	PlayerInputComponent->BindAction(FName(TEXT("Skill_2")), IE_Pressed, this, &ASlashCharacter::UseSkillThree);
	PlayerInputComponent->BindAction(FName(TEXT("Skill_3")), IE_Pressed, this, &ASlashCharacter::UseSkillFour);
	PlayerInputComponent->BindAction(FName(TEXT("DrinkHP")), IE_Pressed, this, &ASlashCharacter::DrinkHPPotion);
	PlayerInputComponent->BindAction(FName(TEXT("DrinkMP")), IE_Pressed, this, &ASlashCharacter::DrinkMPPotion);
	PlayerInputComponent->BindAction(FName(TEXT("Test")), IE_Pressed, this, &ASlashCharacter::TestKey);
}

float ASlashCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (ActionState >= EActionState::EAS_Skill_1 && ActionState <= EActionState::EAS_Skill_4)
	{
		return DamageAmount;
	}
	float DamageAfterBuffMult = DamageAmount * DefenseBuffMultiplier;
	HandleDamage(DamageAfterBuffMult);
	SetHUDHealth();
	return DamageAfterBuffMult;
}

void ASlashCharacter::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	if (ActionState >= EActionState::EAS_Skill_1 && ActionState <= EActionState::EAS_Skill_4)
	{
		return;
	}

	Super::GetHit_Implementation(ImpactPoint, Hitter);
	AttackComboReset();
	SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);
	if (Attributes && Attributes->GetHealthPercent() > 0.f)
	{
		ActionState = EActionState::EAS_HitReaction;
	}
}

void ASlashCharacter::SetOverlappingItem(AItem* Item)
{
	OverlappingItem = Item;
}

void ASlashCharacter::AddSouls(ASoul* Soul)
{
	Attributes->AddSoul(Soul->GetSoul());
	SlashOverlay->SetSouls(Attributes->GetSoul());
}

void ASlashCharacter::AddGold(float Gold)
{
	Attributes->AddGold(Gold);
	SlashOverlay->SetGold(Attributes->GetGold());
}

void ASlashCharacter::Jump()
{
	if (isCutScene) return;
	if (IsUnoccupied())
	{
		Super::Jump();
	}

}

void ASlashCharacter::TurnOnPickupUI()
{
	InteractMessage->TurnOn();
}

void ASlashCharacter::TurnOffPickupUI()
{
	InteractMessage->TurnOff();
}

bool ASlashCharacter::UpdateQuest(const FString& QuestName, int32 QuestUpdatedProgress)
{
	return SlashOverlay->UpdateQuest(QuestName, QuestUpdatedProgress);
}

void ASlashCharacter::AddQuest(const FString& QuestName, const FString& QuestEntryString, int32 QuestObjectiveAmount, int32 QuestCurrentAmount, bool bIsDisplayingNumbers)
{
	SlashOverlay->AddQuest(QuestName, QuestEntryString, QuestObjectiveAmount, QuestCurrentAmount, bIsDisplayingNumbers);
}

bool ASlashCharacter::IsQuestActive(const FString& QuestName)
{
	return SlashOverlay->IsQuestActive(QuestName);
}

int32 ASlashCharacter::GetQuestCurrentProgress(const FString& QuestName)
{
	return SlashOverlay->GetCurrentQuestProgress(QuestName);
}

void ASlashCharacter::BeginPlay()
{
	Super::BeginPlay();

	Tags.Add(FName(TEXT("EngageableTarget")));	// 罹먯뒪?몃? 以꾩씠湲??꾪븳 ?쒓렇 ?ㅼ젙, Enemy PawnSeen 李멸퀬 

	ActorsToIgnore.Add(this);

	Skill4SocketNameReady.Add(FName(TEXT("Skill4Socket_Lv1_0")));
	Skill4SocketNameReady.Add(FName(TEXT("Skill4Socket_Lv1_1")));
	Skill4SocketNameReady.Add(FName(TEXT("Skill4Socket_Lv1_2")));
	Skill4SocketNameReady.Add(FName(TEXT("Skill4Socket_Lv1_3")));
	Skill4SocketNameReady.Add(FName(TEXT("Skill4Socket_Lv1_4")));
	Skill4SocketNameReady.Add(FName(TEXT("Skill4Socket_Lv1_5")));
	Skill4SocketNameReady.Add(FName(TEXT("Skill4Socket_Lv2_0")));
	Skill4SocketNameReady.Add(FName(TEXT("Skill4Socket_Lv2_1")));

	InitializeSlashOverlay();

	CameraDefaultHeight = ViewCamera->GetRelativeLocation().Z;
	CameraSkill4Height = CameraDefaultHeight + 40.f;

	Attributes->ManaRegenPerSecond();
	//SkillOneHitbox->Equip(GetMesh(), FName(TEXT("Skill1Socket")), this, this);

	UWorld* World = GetWorld();

	if (World)
	{

		FString BlueprintPath = TEXT("/Game/Blueprints/Item/Weapon/BP_SkillOneHitbox.BP_SkillOneHitbox_C");
		UClass* BlueprintClass = Cast<UClass>(StaticLoadObject(UClass::StaticClass(), nullptr, *FName(*BlueprintPath).ToString()));

		if (BlueprintClass)
		{
			FActorSpawnParameters ActorSpawnParameters;
			ActorSpawnParameters.Instigator = this;
			FVector SocketLocation = GetMesh()->GetSocketLocation(FName("Skill1Socket"));
			FRotator SocketRotation = GetMesh()->GetSocketRotation(FName("Skill1Socket"));

			// 월드에서 액터를 스폰합니다.
			SkillOneHitbox = World->SpawnActor<AEnemyWeapon>(BlueprintClass, SocketLocation, SocketRotation, ActorSpawnParameters);
			if (SkillOneHitbox)
			{
				SkillOneHitbox->SetOwner(this);
				SkillOneHitbox->Equip(GetMesh(), FName("Skill1Socket"), this, this);
			}
		}
	}
}

void ASlashCharacter::MoveForward(float Value)
{
	if (isCutScene) return;
	if (ActionState != EActionState::EAS_Unoccupied)
	{
		return;
	}

	if (Controller && Value != 0.f)
	{
		//FVector Forward = GetActorForwardVector();
		//AddMovementInput(Forward, Value);

		const FRotator ControlRotation = GetControlRotation();		// 而⑦듃濡ㅻ윭???뚯쟾媛?
		const FRotator YawRotation(0.f, ControlRotation.Yaw, 0.f);	// 而⑦듃濡ㅻ윭???뚯쟾媛믪뿉??Yaw媛믩쭔 ?곗삩 寃?

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);	// YawRotation??湲곗??쇰줈 ???뚯쟾?됰젹?먯꽌 Look 諛⑺뼢???⑥쐞踰≫꽣瑜?媛?몄샂
		AddMovementInput(Direction, Value);
	}
}

void ASlashCharacter::MoveRight(float Value)
{
	if (isCutScene) return;
	if (ActionState != EActionState::EAS_Unoccupied)
	{
		return;
	}

	if (Controller && Value != 0.f)
	{
		//FVector Right = GetActorRightVector();
		//AddMovementInput(Right, Value);

		const FRotator ControlRotation = GetControlRotation();		// 而⑦듃濡ㅻ윭???뚯쟾媛?
		const FRotator YawRotation(0.f, ControlRotation.Yaw, 0.f);	// 而⑦듃濡ㅻ윭???뚯쟾媛믪뿉??Yaw媛믩쭔 ?곗삩 寃?

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);	// YawRotation??湲곗??쇰줈 ???뚯쟾?됰젹?먯꽌 Right 諛⑺뼢???⑥쐞踰≫꽣瑜?媛?몄샂
		AddMovementInput(Direction, Value);
	}
}

void ASlashCharacter::Turn(float Value)
{
	if (isCutScene) return;
	AddControllerYawInput(Value);
}

void ASlashCharacter::LookUp(float Value)
{
	if (isCutScene) return;
	AddControllerPitchInput(Value);
}

void ASlashCharacter::EKeyPressed()
{
	if (isCutScene) return;
	AWeapon* OverlappingWeapon = Cast<AWeapon>(OverlappingItem);

	if (OverlappingWeapon)	// 1. ?붾뱶???볦뿬 ?덈뒗 臾닿린??肄쒕━??援ъ껜? ?뚮젅?댁뼱媛 寃뱀낀??寃쎌슦
	{
		EquipWeapon(OverlappingWeapon);
	}
}

void ASlashCharacter::Equip()
{
	if (isCutScene) return;
	if (CanDisarm())	// 무기를 집어넣음
	{
		Disarm();
	}

	else if (CanArm())	// 臾닿린瑜?爰쇰깂
	{
		Arm();
	}
}

void ASlashCharacter::Attack()
{
	if (isCutScene) return;
	if (ActionState == EActionState::EAS_Attack_1 || ActionState == EActionState::EAS_Attack_2 || ActionState == EActionState::EAS_Attack_3)
	{
		if (bAttackable && !bAttackCount[0] && ActionState == EActionState::EAS_Attack_1)
		{
			bAttackCount[0] = true;
			AttackMontageIndex = (AttackMontageIndex + 1) % GetAttackMontageSectionsNum();
		}
		else if (bAttackable && bAttackCount[0] && !bAttackCount[1] && ActionState == EActionState::EAS_Attack_2)
		{
			bAttackCount[1] = true;
			AttackMontageIndex = (AttackMontageIndex + 1) % GetAttackMontageSectionsNum();
		}
		return;
	}

	Super::Attack();

	if (CanAttack())
	{

		bAttackable = false;
		AttackMontageIndex = 0;
		PlayAttackMontage();	// 怨듦꺽 ?좊땲硫붿씠??紐쏀?二??ъ깮
		ActionState = EActionState::EAS_Attack_1;

	}
}

void ASlashCharacter::AttackCombo()
{
	if (!bAttacking[0]) bAttacking[0] = true;
	else if (bAttacking[0] && !bAttacking[1]) bAttacking[1] = true;

	bAttackable = false;
	PlayAttackMontage();	// 怨듦꺽 ?좊땲硫붿씠??紐쏀?二??ъ깮

}

void ASlashCharacter::RefillCooldown()
{
	if (isCutScene) return;
	Skill->RefillCooldown();
}

void ASlashCharacter::DrinkHPPotion()
{
	if (isCutScene) return;
	int32 newHPPotionAmount = Attributes->GetHPPotion() - 1;

	if (newHPPotionAmount >= 0)
	{
		Attributes->SetHPPotion(newHPPotionAmount);
		SlashOverlay->SetHPPotion(newHPPotionAmount);
		Attributes->HealingHP(Attributes->GetHPPotionHealingAmount());
		SetHUDHealth();
		if (IsValid(PotionSound))
		{
			UGameplayStatics::PlaySound2D(GetWorld(), PotionSound);
		}
	}
}

void ASlashCharacter::DrinkMPPotion()
{
	if (isCutScene) return;
	int32 newMPPotionAmount = Attributes->GetMPPotion() - 1;

	if (newMPPotionAmount >= 0)
	{
		Attributes->SetMPPotion(newMPPotionAmount);
		SlashOverlay->SetMPPotion(newMPPotionAmount);
		Attributes->HealingMP(Attributes->GetMPPotionHealingAmount());
		SetHUDMana();
		if (IsValid(PotionSound))
		{
			UGameplayStatics::PlaySound2D(GetWorld(), PotionSound);
		}
	}
}

void ASlashCharacter::Die()
{
	Super::Die();

	ActionState = EActionState::EAS_Dead;
	DisableMeshCollision();
}

void ASlashCharacter::Dodge()
{
	if (isCutScene) return;
	if (ActionState == EActionState::EAS_HitReaction || ActionState > EActionState::EAS_Attack_3 || Attributes->IsDodgeChargeZero())
	{
		return;
	}
	PlayDodgeMontage(TEXT("DodgeFront"));
	ActionState = EActionState::EAS_Dodge;	// ?뚰뵾 ?좊땲硫붿씠??紐쏀?二쇱뿉??DodgeEnd ?대깽???ㅼ젙?댁쨾?쇳븿

	if (Attributes->IsDodgeChargeFull())
	{
		Attributes->ResetDodgeChargeCooldown();
	}

	Attributes->DecreaseDodgeCharge();
	PreviousMeshPos = GetMesh()->GetComponentLocation();
}

void ASlashCharacter::Roll()
{
	if (isCutScene) return;
	if (ActionState == EActionState::EAS_HitReaction || ActionState > EActionState::EAS_Attack_3 || !Attributes->IsRollCooldownOver() || !Attributes->IsSufficientMana(10.f))
	{
		return;
	}

	PlayDodgeMontage(FName(TEXT("DodgeRoll")));
	ActionState = EActionState::EAS_Roll;
	ResetRollCooldown();
	Attributes->UseMana(10.f);
	SlashOverlay->SetManaBarPercent(Attributes->GetManaPercent());
	PreviousMeshPos = GetMesh()->GetComponentLocation();
}

void ASlashCharacter::ResetRollCooldown()
{
	Attributes->ResetRollCooldown();
}

void ASlashCharacter::TestKey()
{
	if (isCutScene) return;
	SlashOverlay->UpdateQuest(FString(TEXT("FirstQuest")), 1);
	SlashOverlay->AddQuest(FString(TEXT("SecondQuest")), FString(TEXT("Second Quest Test to check CompleteQuest system is working")), 1, 0, false);
}

void ASlashCharacter::EquipWeapon(AWeapon* Weapon)
{
	Weapon->Equip(GetMesh(), FName(TEXT("RightHandSocket")), this, this);
	CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
	OverlappingItem = nullptr;	// 臾닿린瑜??μ갑?????뚮젅?댁뼱媛 ???댁긽 臾닿린? 寃뱀튂吏 ?딅뒗?ㅻ뒗 泥섎━瑜??섍린 ?꾪빐 nullptr 泥섎━
	EquippedWeapon[0] = Weapon;	// ?μ갑??臾닿린??EquippedWeapon?쇰줈 媛由ы궎???ъ씤?곕? 蹂寃?
	TurnOffPickupUI();
}

void ASlashCharacter::AttackEnd()	// 怨듦꺽???앸굹???쒖젏???뚮젅?댁뼱??ActionState瑜?EAS_Unoccupied濡??ㅼ떆 ?섎룎由щ뒗 肄쒕갚 ?⑥닔
{
	if (bAttackable)
	{
		if (bAttackCount[0] && !bAttackCount[1] && ActionState == EActionState::EAS_Attack_1)
		{
			ActionState = EActionState::EAS_Attack_2;
			return;
		}
		else if (bAttackCount[0] && bAttackCount[1] && ActionState == EActionState::EAS_Attack_2)
		{
			ActionState = EActionState::EAS_Attack_3;
			return;
		}
	}
	AttackComboReset();

	ActionState = EActionState::EAS_Unoccupied;
}

void ASlashCharacter::AttackComboReset()
{
	//肄ㅻ낫 珥덇린??
	bAttackCount[0] = false;
	bAttackCount[1] = false;
	bAttacking[0] = false;
	bAttacking[1] = false;
}

bool ASlashCharacter::CanAttack()
{
	//(ActionState == static_cast<EActionState>(static_cast<int32>(EActionState::EAS_Attack_1) + (AttackMontageIndex + 3) % 3))) &&
	return ActionState == EActionState::EAS_Unoccupied &&
		CharacterState != ECharacterState::ECS_Unequipped;
}

void ASlashCharacter::DodgeEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
}

void ASlashCharacter::RollEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
}

void ASlashCharacter::JumpEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
}

bool ASlashCharacter::CanDisarm()
{
	return CharacterState != ECharacterState::ECS_Unequipped &&
		ActionState == EActionState::EAS_Unoccupied;
}

bool ASlashCharacter::CanArm()
{
	return ActionState == EActionState::EAS_Unoccupied &&
		CharacterState == ECharacterState::ECS_Unequipped &&
		EquippedWeapon[0];
}

void ASlashCharacter::Disarm()
{
	PlayEquipMontage(FName(TEXT("Unequip")));
	CharacterState = ECharacterState::ECS_Unequipped;
	ActionState = EActionState::EAS_EquippingWeapon;
}

void ASlashCharacter::Arm()
{
	PlayEquipMontage(FName(TEXT("Equip")));
	CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
	ActionState = EActionState::EAS_EquippingWeapon;
}

bool ASlashCharacter::IsAttacking()
{
	return ActionState == static_cast<EActionState>(static_cast<int32>(EActionState::EAS_Attack_1) + AttackMontageIndex);
}

bool ASlashCharacter::IsAttackMontageIndexOverflowed()
{
	return AttackMontageIndex >= GetAttackMontageSectionsNum() - 1;
}

void ASlashCharacter::UseSkillOne()
{
	if (isCutScene) return;
	if (CanAttack() && Skill && Skill->IsSkillOneAvailable() && Attributes->IsSufficientMana(Skill->SkillOneManaConsumption))
	{
		Skill->PlaySkillMontage(EActionState::EAS_Skill_1);
		ActionState = EActionState::EAS_Skill_1;
		PreviousMeshPos = GetMesh()->GetComponentLocation();
		if (Skill->IsSkillOneChargeFull())
		{
			Skill->ResetCooldown(EActionState::EAS_Skill_1);
		}
		Skill->DecreaseSkillOneCharge();
		SlashOverlay->SetSkillOneChargeCounter(Skill->GetSkillOneCharge());
		Attributes->UseMana(Skill->SkillOneManaConsumption);
		SlashOverlay->SetManaBarPercent(Attributes->GetManaPercent());
	}
}

void ASlashCharacter::UseSkillTwo()
{
	if (CanAttack() && Skill && Skill->IsSkillAvailable(EActionState::EAS_Skill_2))
	{
		Skill->PlaySkillMontage(EActionState::EAS_Skill_2);
		ActionState = EActionState::EAS_Skill_2;
	}
}

void ASlashCharacter::UseSkillThree()
{
	if (isCutScene) return;
	if (CanAttack() && Skill && Skill->IsSkillAvailable(EActionState::EAS_Skill_3) && Attributes->IsSufficientMana(Skill->SkillThreeManaConsumption))
	{
		SkillThreeEffect->Activate();
		SkillThreeRemainingTime = SkillThreeEffectTime;

		// ?뚮젅?댁뼱 怨듦꺽?? 諛⑹뼱?? 泥대젰 ?꾪듃??
		if (EquippedWeapon.Num() > 0)
		{
			for (auto& Weapon : EquippedWeapon)
			{
				if (IsValid(Weapon))
				{
					Weapon->BuffOn();
				}
			}
		}

		DefenseBuffMultiplier = 0.5f;
		bIsSkillThreeActive = true;
		SlashOverlay->SetSkillThreeActiveIcon(true);
		Skill->ResetCooldown(EActionState::EAS_Skill_3);
		Attributes->UseMana(Skill->SkillThreeManaConsumption);
		SlashOverlay->SetManaBarPercent(Attributes->GetManaPercent());
	}
}

void ASlashCharacter::UseSkillFour()
{
	if (isCutScene) return;
	if (CanAttack() && Skill && Skill->IsSkillAvailable(EActionState::EAS_Skill_4) && Attributes->IsSufficientMana(Skill->SkillFourManaConsumption))
	{
		Skill->PlaySkillMontage(EActionState::EAS_Skill_2);
		ActionState = EActionState::EAS_Skill_4;
		Skill->ResetCooldown(ActionState);
		Attributes->UseMana(Skill->SkillFourManaConsumption);
		SlashOverlay->SetManaBarPercent(Attributes->GetManaPercent());
	}
}

bool ASlashCharacter::IsSkillActive(int32 SkillIndex)
{
	return ActionState == static_cast<EActionState>(static_cast<int32>(EActionState::EAS_Skill_1) + SkillIndex);
}

int32 ASlashCharacter::GetCurrentSkillLevel(EActionState ActionStateToCheck)
{
	return Skill->GetSkillLevel(ActionStateToCheck);
}

void ASlashCharacter::Skill4Shoot()
{
	if (Skill && ActionState == EActionState::EAS_Skill_4)
	{
		Skill->Shoot();

		FName FreedSocket = Skill4SocketNameFired[0];
		Skill4SocketNameFired.RemoveAt(0);
		Skill4SocketNameReady.Add(FreedSocket);
	}
}

void ASlashCharacter::Skill4Spawn()
{
	if (Skill4SocketNameReady.Num() <= 0)
	{
		return;
	}
	if (Skill && ActionState == EActionState::EAS_Skill_4)
	{
		FName SelectedSocket = Skill4SocketNameReady[0];
		Skill4SocketNameReady.RemoveAt(0);
		Skill4SocketNameFired.Add(SelectedSocket);
		FVector Location = GetMesh()->GetSocketLocation(SelectedSocket);
		FRotator Rotation = GetMesh()->GetSocketRotation(SelectedSocket);
		Skill->Spawn(Location, Rotation, this);
	}
}

void ASlashCharacter::SkillEnd()
{
	bSkillInvincibility = false;
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Block);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
	ActionState = EActionState::EAS_Unoccupied;
}

void ASlashCharacter::SetSkillCollisionEnabled(ECollisionEnabled::Type CollisionEnabled)
{
	if (IsValid(SkillOneHitbox))
	{
		SkillOneHitbox->GetWeaponBox()->SetCollisionEnabled(CollisionEnabled);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("SkilllOneHitbox is invalid"))
		return;
	}

	if (CollisionEnabled == ECollisionEnabled::NoCollision)
	{
		ActorsToIgnore.Empty();
		ActorsToIgnore.Add(this);
	}
}

void ASlashCharacter::AttackChainEnd()
{
	bAttackable = true;
}

void ASlashCharacter::HitReactEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
}

void ASlashCharacter::PlayEquipMontage(const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && EquipMontage)
	{
		AnimInstance->Montage_Play(EquipMontage);	// ?μ갑 ?좊땲硫붿씠??紐쏀?二??ъ깮
		AnimInstance->Montage_JumpToSection(SectionName, EquipMontage);	// ?좏깮???뱀뀡 ?ㅼ엫?쇰줈 紐쏀?二쇱쓽 ?ъ깮 ?쒖젏 ?대룞
	}
}

void ASlashCharacter::PlayDodgeMontage(const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && DodgeMontage)
	{
		AnimInstance->Montage_Play(DodgeMontage);
		AnimInstance->Montage_JumpToSection(SectionName, DodgeMontage);
	}
}

int32 ASlashCharacter::PlayAttackMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	UAnimMontage* Montage = GetAttackMontage();

	FName SectionName = GetAttackMontageSectionName(AttackMontageIndex);

	if (AnimInstance && Montage)
	{
		AnimInstance->Montage_Play(Montage);
		AnimInstance->Montage_JumpToSection(SectionName, Montage);
	}
	return AttackMontageIndex;
}

void ASlashCharacter::AttachWeaponToBack()
{
	if (EquippedWeapon[0])
	{
		EquippedWeapon[0]->AttachMeshToSocket(GetMesh(), FName(TEXT("SpineSocket")));
	}
}

void ASlashCharacter::AttachWeaponToHand()
{
	if (EquippedWeapon[0])
	{
		EquippedWeapon[0]->AttachMeshToSocket(GetMesh(), FName(TEXT("RightHandSocket")));
	}
}

void ASlashCharacter::FinishEquipping()
{
	ActionState = EActionState::EAS_Unoccupied;
}

void ASlashCharacter::InitializeSlashOverlay()
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());

	if (PlayerController)
	{
		ASlashHUD* SlashHUD = Cast<ASlashHUD>(PlayerController->GetHUD());

		if (SlashHUD)
		{
			SlashOverlay = SlashHUD->GetSlashOverlay();
			if (SlashOverlay)
			{
				SetHUDHealth();
				SetHUDMana();
				SlashOverlay->SetGold(0);
				SlashOverlay->SetSouls(0);
				SlashOverlay->SetHPPotion(3);
				SlashOverlay->SetMPPotion(3);
				SlashOverlay->SetSkillThreeActiveIcon(false);
				SlashOverlay->SetSkillOneBarPercent(0.f);
				SlashOverlay->SetSkillThreeBarPercent(0.f);
				SlashOverlay->SetSkillFourBarPercent(0.f);
			}
		}
	}

	UWorld* World = GetWorld();

	if (World)
	{
		if (PlayerController && InteractMessageClass)
		{
			InteractMessage = CreateWidget<UInteractMessage>(PlayerController, InteractMessageClass);
			if (IsValid(InteractMessage))
			{
				InteractMessage->TurnOff();
			}
		}
	}

	SlashOverlay->SetSkillOneCooldownCounter(-1.f);
	SlashOverlay->SetSkillThreeCooldownCounter(-1.f);
	SlashOverlay->SetSkillFourCooldownCounter(-1.f);
	SlashOverlay->SetSkillThreeActiveCounter(-1.f);

	SlashOverlay->AddQuest(FString(TEXT("마을 조사")), FString(TEXT("숲 옆에 있는 마을을 찾으십시오.")), 1, 0, false);
	SlashOverlay->ShowQuestUI();
}

void ASlashCharacter::SetHUDHealth()
{
	if (SlashOverlay && Attributes)
	{
		SlashOverlay->SetHealthBarPercent(Attributes->GetHealthPercent());
		SlashOverlay->SetHPCounter(Attributes->GetHP(), Attributes->GetMaxHP());
	}
}

void ASlashCharacter::SetHUDMana()
{
	if (SlashOverlay && Attributes)
	{
		SlashOverlay->SetManaBarPercent(Attributes->GetManaPercent());
		SlashOverlay->SetMPCounter(Attributes->GetMP(), Attributes->GetMaxMP());
	}
}

bool ASlashCharacter::IsUnoccupied()
{
	return ActionState == EActionState::EAS_Unoccupied;
}



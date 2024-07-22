#pragma once

UENUM(BlueprintType)	// BlueprintType : �������Ʈ�� enum�� ���̰� �ϴ� ��ũ��, BlueprintType
enum class ECharacterState : uint8	// enum ���� uint8�� �ٿ��� ����� ������ �� ����
{
	ECS_Unequipped UMETA(DisplayName = "Unequipped"),	// UMETA(DisplayName = ""): �������Ʈ�� enum �׸����� ǥ���� �̸��� ���� �� ����
	ECS_EquippedOneHandedWeapon UMETA(DisplayName = "Equipped One-Handed Weapon"),
	ECS_EquippedTwoHandedWeapon UMETA(DisplayName = "Equipped Two-Handed Weapon")
};

UENUM(BlueprintType)
enum class EActionState : uint8
{
	EAS_Unoccupied UMETA(DisplayName = "Unoccupied"),
	EAS_HitReaction UMETA(DisplayName = "Hit Reaction"),
	EAS_Attack_1 UMETA(DisplayName = "Attack_1"),
	EAS_Attack_2 UMETA(DisplayName = "Attack_2"), 
	EAS_Attack_3 UMETA(DisplayName = "Attack_3"),
	EAS_Skill_1 UMETA(DisplayName = "Skill_1"),
	EAS_Skill_2 UMETA(DisplayName = "Skill_2"),
	EAS_Skill_3 UMETA(DisplayName = "Skill_3"),
	EAS_Skill_4 UMETA(DisplayName = "Skill_4"),
	EAS_EquippingWeapon UMETA(DisplayName = "Equipping Weapon"),
	EAS_Dodge UMETA(DisplayName = "Dodge"),
	EAS_Roll UMETA(DisplayName = "Roll"),
	EAS_Jump UMETA(DisplayName = "Jump"),
	EAS_Dead UMETA(DisplayName = "Dead")
};

UENUM(BlueprintType)
enum EDeathPose
{
	EDP_Death1 UMETA(DisplayName = "Death1"),
	EDP_Death2 UMETA(DisplayName = "Death2"),
	EDP_Death3 UMETA(DisplayName = "Death3"),
	EDP_Death4 UMETA(DisplayName = "Death4"),
	EDP_MAX UMETA(DisplayName = "DefaultMax")
};

UENUM(BlueprintType)
enum class EEnemyState : uint8
{
	EES_NoState UMETA(DisplayName = "NoState"),
	EES_Dead UMETA(DisplayName = "Dead"),
	EES_Patrolling UMETA(DisplayName = "Patrolling"),
	EES_Chasing UMETA(DisplayName = "Chasing"),
	EES_Attacking UMETA(DisplayName = "Attacking"),
	EES_Engaged UMETA(DisplayName = "Engaged"),
	EES_Skill1 UMETA(DisplayName = "Skill1"),
	EES_Skill2 UMETA(DisplayName = "Skill2"),
	EES_Skill3 UMETA(DisplayName = "Skill3"),
	EES_Skill4 UMETA(DisplayName = "Skill4")
};
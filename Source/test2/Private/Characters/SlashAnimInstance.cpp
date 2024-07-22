// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/SlashAnimInstance.h"
#include "Characters/SlashCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

USlashAnimInstance::USlashAnimInstance() : SlashCharacter(nullptr), SlashCharacterMovement(nullptr), GroundSpeed(0.f), IsFalling(false), CharacterState(ECharacterState::ECS_Unequipped)
{

}

void USlashAnimInstance::NativeInitializeAnimation()	// �ִϸ��̼� �ν��Ͻ��� �ʱ�ȭ�� �� ����Ǵ� �Լ�
{
	Super::NativeInitializeAnimation();

	SlashCharacter = Cast<ASlashCharacter>(TryGetPawnOwner());	// Cast : �𸮾󿡼� �����ϴ� ĳ��Ʈ �Լ�

	if (SlashCharacter)
	{
		SlashCharacterMovement = SlashCharacter->GetCharacterMovement();	// GetCharacterMovement : ĳ������ Movement ������Ʈ�� �������� �Լ�
	}
}

void USlashAnimInstance::NativeUpdateAnimation(float DeltaTime)	// �ִϸ��̼� ƽ���� ���ư��� �Լ�
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (SlashCharacterMovement)
	{
		FVector Velocity = SlashCharacterMovement->Velocity;
		GroundSpeed = UKismetMathLibrary::VSizeXY(Velocity);
		IsFalling = SlashCharacterMovement->IsFalling();	// Movement ������Ʈ�� ����Ǿ��ִ� �Լ�, ĳ���Ͱ� ���߿� �ִ��� �ƴ����� �Ǻ�
		CharacterState = SlashCharacter->GetCharacterState();
		ActionState = SlashCharacter->GetActionState();
		DeathPose = SlashCharacter->GetDeathPose();
	}
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/SlashAnimInstance.h"
#include "Characters/SlashCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

USlashAnimInstance::USlashAnimInstance() : SlashCharacter(nullptr), SlashCharacterMovement(nullptr), GroundSpeed(0.f), IsFalling(false), CharacterState(ECharacterState::ECS_Unequipped)
{

}

void USlashAnimInstance::NativeInitializeAnimation()	// 애니메이션 인스턴스가 초기화될 때 실행되는 함수
{
	Super::NativeInitializeAnimation();

	SlashCharacter = Cast<ASlashCharacter>(TryGetPawnOwner());	// Cast : 언리얼에서 제공하는 캐스트 함수

	if (SlashCharacter)
	{
		SlashCharacterMovement = SlashCharacter->GetCharacterMovement();	// GetCharacterMovement : 캐릭터의 Movement 컴포넌트를 가져오는 함수
	}
}

void USlashAnimInstance::NativeUpdateAnimation(float DeltaTime)	// 애니메이션 틱마다 돌아가는 함수
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (SlashCharacterMovement)
	{
		FVector Velocity = SlashCharacterMovement->Velocity;
		GroundSpeed = UKismetMathLibrary::VSizeXY(Velocity);
		IsFalling = SlashCharacterMovement->IsFalling();	// Movement 컴포넌트에 내장되어있는 함수, 캐릭터가 공중에 있는지 아닌지를 판별
		CharacterState = SlashCharacter->GetCharacterState();
		ActionState = SlashCharacter->GetActionState();
		DeathPose = SlashCharacter->GetDeathPose();
	}
}

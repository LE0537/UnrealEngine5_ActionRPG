// Fill out your copyright notice in the Description page of Project Settings.

#include "Pawns/Bird.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

ABird::ABird()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));

	Capsule->SetCapsuleHalfHeight(20.f);	// ĸ���� ���� ���̸� �����ϴ� �Լ�
	Capsule->SetCapsuleRadius(15.f);		// ĸ���� ������ ���̸� �����ϴ� �Լ�

	SetRootComponent(Capsule);	// == RootComponent = Capsule;

	BirdMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BirdMesh"));
	BirdMesh->SetupAttachment(GetRootComponent());	// ���� �޽ø� ��Ʈ ������Ʈ�� ���̴�(Attach) �Լ�, �̰� �� �ϸ� ��Ʈ ������Ʈ�� �������� �޽ô� ���ڸ��� ���� 

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));	// ���� �������Ʈ���� ������ �׸��� �� �߸� ���� �̸��� �� �� �ٲ����
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 150.f;											// ���������� ����

	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));
	ViewCamera->SetupAttachment(CameraBoom);

	AutoPossessPlayer = EAutoReceiveInput::Player0;	// �÷��̾ �ش� ������ �ڵ����� ����(Possess)�ϵ��� �ϴ� ����
}

void ABird::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABird::MoveForward(float Value)	// �� �����Ӹ��� ȣ��
{
	if (Controller && (Value != 0.f))
	{
		FVector Forward = GetActorForwardVector();
		AddMovementInput(Forward, Value);	// ��Ʈ�ѷ��� �ִ� �̵�(Movement) ������Ʈ�� ������ �����ϴ� �Լ�
	}
}

void ABird::Turn(float Value)
{
	AddControllerYawInput(Value);
}

void ABird::LookUp(float Value)
{
	AddControllerPitchInput(Value);
}

void ABird::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABird::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(FName(TEXT("MoveForward")), this, &ABird::MoveForward);	// �Լ��� ��ǲ�� ��������ִ� �Լ�
																							// ������Ʈ �������� ���� �� �Է� �̸�, �� ��ü, ������ �Լ��� �ּҸ� �Է��ؾ� ��
	PlayerInputComponent->BindAxis(FName(TEXT("Turn")), this, &ABird::Turn);				// ���콺 �¿�(yaw) ȸ�� �Լ� ����
	PlayerInputComponent->BindAxis(FName(TEXT("LookUp")), this, &ABird::LookUp);			// ���콺 ����(pitch) ȸ�� �Լ� ����
}


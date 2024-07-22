// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

class USphereComponent;
class UNiagaraComponent;
class UNiagaraSystem;
class UInteractMessage;

enum class EItemState : uint8	// ��ü�� �յ� ���ٴϴ� ó���� ���� ������ ��Ÿ���� enum class
{
	EIS_Hovering,	// �������� ���ٴ�
	EIS_Equipped	// �÷��̾ �������� ������ ���¶� ���ٴ��� ����
};

UCLASS()
class TEST2_API AItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItem();
	void SetItemState(EItemState State) { ItemState = State; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sine Parameter")	//BlueprintReadOnly : �������Ʈ�� Getter ��� ����
																				//BlueprintReadWrite : �������Ʈ�� Getter, Setter ��� ����
	float Amplitude;															//private ���������� �۵����� �����Ƿ� protected�� public�̿��� ��

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sine Parameter")	//Category : �������Ʈ �������� Detail�� ī�װ���� �߰��ϰų� ���� ī�װ���� ����
	float TimeConstant;

	//UFUNCTION(BlueprintCallable);	//�Լ��� �������Ʈ�� �����ϴ� ��ũ��
									//BlueprintCallable : �Լ��� �������Ʈ�� ������
	UFUNCTION(BlueprintPure)		//BlueprintPure : �Լ��� �������Ʈ�� �����Լ��� ������
	float TransformedSin(float Value);

	UFUNCTION(BlueprintPure)
	float TransformedCos(float Value);

	template<typename T>
	T Avg(T First, T Second);

	UFUNCTION()	// UFUNCTION�� �ٿ��� ����� ��������Ʈ�� �۵���
				// OnSphereOverlap�� Sphere�� ������ �浹�� �˻��ϴµ�, �� Sphere�� ������ �������� ���� ������Ʈ���� ����� �� �ִ� PrimitiveComponent.h���� 
				// �̺�Ʈ���� �̸��� �� ��������Ʈ ��ũ�θ� ã�ƺ� �� �ִ�. �� �� �츮�� ����ϰ� ���� OnComponentBeginOverlap �̺�Ʈ�� ���� ��������Ʈ�� Ȯ���ϰ�
				// �̺�Ʈ �̸� ���� �ִ� �Ű������� Ÿ�� �̸���� ���� �̸����� �����Ͽ� �츮�� ����� �Լ�(OnSphereOverlap)�� �Ű������� �����.
				// OnComponentBeginOverlap�� 6���� �Ű������� �ʿ��� ��������Ʈ��, ������ 6���� �Ű������� �츮 �Լ��� �����Ѵ�.
				// ���� BeginPlay���� AddDynamic ��ũ�θ� �̿��Ͽ� �츮�� ���� �ݹ� �Լ��� ��������Ʈ�� �����Ѵ�.
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()	// OnSphereEndOverlap�� �Լ��� �� �̻� ���������� �ʴ� ������ ����Ǵ� �̺�Ʈ�̸�, �� �̺�Ʈ�� PrimitiveComponent.h����
				// �Ű����� 4���� ���� ��������Ʈ ��ũ�ο� ����Ǿ� �ִ�. ���� �ش� ��ũ�ο� �����ִ� �Ű����� Ÿ�Ե�� �̸����� �����Ͽ� �Լ��� �ۼ��ϰ�
				// ���� BeginPlay���� AddDynamic ��ũ�θ� �̿��Ͽ� �츮�� ���� �ݹ� �Լ��� ��������Ʈ�� �����Ѵ�.
	virtual void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	virtual void SpawnPickupEffect();
	virtual void SpawnPickupSound();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* ItemMesh;

	EItemState ItemState;

	UPROPERTY(EditAnywhere)
	UNiagaraComponent* ItemEffect;

	UPROPERTY(VisibleAnywhere)
	USphereComponent* Sphere;	// �浹(������)�� ������ �� ����� ��ü ������Ʈ

	//UPROPERTY(EditDefaultsOnly)
	//UInteractMessage* InteractMessage;

	//UPROPERTY(EditDefaultsOnly)
	//TSubclassOf<UInteractMessage> InteractMessageClass;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	//void ResetUI();

private:
	//EditDefalutsOnly : ����Ʈ �������Ʈ������ ���� ������ �� ������ ���忡 �ִ� �ν��Ͻ������� �Ұ�����
	//EditInstanceOnly : ���忡 �ִ� �ν��Ͻ������� ���� ������ �� ������ ����Ʈ �������Ʈ������ �Ұ�����
	//EditAnywhere : ����Ʈ �������Ʈ�� ���忡 �ִ� �ν��Ͻ� �� �ٿ��� ���ų� ������ �� ����
	//VisibleDefaultOnly : ����Ʈ �������Ʈ���� �� ���� ������ ������ �Ұ�����
	//VisibleInstanceOnly : ���忡 �ִ� �ν��Ͻ������� �� �� ������ ������ �Ұ�����
	//VisibleAnywhere : ����Ʈ �������Ʈ�� ���忡 �ִ� �ν��Ͻ� �� �ٿ��� �� �� ������ ������ �Ұ�����

	//UPROPERTY(EditAnywhere);
	//UPROPERTY(VisibleInstanceOnly);
	//float RunningTime;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))	//<- private ������ �������Ʈ�� ���̰� �ϴ� ����
	float RunningTime;

	//UPROPERTY(EditAnywhere);
	//float Amplitude;

	//UPROPERTY(EditAnywhere);
	//float TimeConstant;

	UPROPERTY(EditAnywhere)
	UNiagaraSystem* PickupEffect;

	UPROPERTY(EditAnywhere)
	USoundBase* PickupSound;
};

template<typename T>
inline T AItem::Avg(T First, T Second)
{
	return (First + Second) / 2;
}

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Weapon.generated.h"

/**
 * 
 */

class USoundBase;
class UBoxComponent;

UCLASS()
class TEST2_API AWeapon : public AItem
{
	GENERATED_BODY()

public:
	AWeapon();
	void Equip(USceneComponent* InParent, FName InSocketName, AActor* NewOwner, APawn* NewInstigator);
	void DeactivateEmbers();
	void DisableSphereCollision();
	void PlayEquipSound();
	void AttachMeshToSocket(USceneComponent* InParent, const FName& InSocketName);		// �Լ� ���빰�� �������� �� Ctrl + . �� ������ "�Լ� ����" ����� ����ϸ� �̷��� ���ڰ� ���ش�
	void BuffOn();
	void BuffOff();

protected:
	virtual void BeginPlay() override;

	void Init();
	UFUNCTION()	//
	virtual void OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	bool ActorIsSameType(AActor* OtherActor);

	void ExecuteGetHit(FHitResult& BoxHit);

	void ExecuteGetHitEnemy(AActor* BoxHit);

	UFUNCTION(BlueprintImplementableEvent)
	void CreateFields(const FVector& FieldLocation);

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	UBoxComponent* WeaponBox;	// ������ ���� ������ ����� �ݶ��̴�

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	float Damage = 40.f;
	TArray<AActor*> IgnoreActors;

	float BuffMultiplier = 1.f;

private:
	void BoxTrace(FHitResult& BoxHit);

	UPROPERTY(EditAnywhere)
	bool bShowBoxDebug;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	FVector BoxTraceExtent;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	USoundBase* EquipSound;		// ���⸦ �̰ų� ������� �� �� ����



	UPROPERTY(VisibleAnywhere)	// VisibleAnywhere�� ������Ʈ ���� ���� �ַ� ����, EditAnywhere�� �⺻�������� ��쿡 ����
	USceneComponent* BoxTraceStart;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* BoxTraceEnd;
	
public:
	FORCEINLINE UBoxComponent* GetWeaponBox() const { return WeaponBox; }
	FORCEINLINE void ClearIgnoreActors() { IgnoreActors.Empty(); }
};

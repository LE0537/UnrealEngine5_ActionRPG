// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/HitInterface.h"
#include "Breakable.generated.h"

class UGeometryCollectionComponent;
class UCapsuleComponent;
class ATreasure;

UCLASS()
class TEST2_API ABreakable : public AActor, public IHitInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABreakable();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter);

	UFUNCTION()
	void OnChaosBreakEvent(const FChaosBreakEvent& BreakEvent);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UCapsuleComponent* Capsule;

private:
	UPROPERTY(VisibleAnywhere)
	UGeometryCollectionComponent* GeometryCollection;

	//UPROPERTY(EditAnywhere)
	//UClass* TreasureClass;	// �׾Ƹ��� ���� ���� ����

	//UPROPERTY(EditAnywhere, Category = "Breakable Properties")
	//TSubclassOf<ATreasure> TreasureClass;	// UClass ������ �ƹ� Ŭ������ ���� �� �־� �Ǽ��� ���ɼ��� ������
											// TSubclassOf�� UClass�� ��ȯ�ϸ鼭�� ATreasure�� �Ļ�Ŭ������ �޾Ƶ���

	UPROPERTY(EditAnywhere, Category = "Breakable Properties")
	TArray<TSubclassOf<ATreasure>> TreasureClasses;		// ���� ������Ʈ�� �߿��� �������� ������ �� ���̹Ƿ� ����(TArray)�� ���� �������Ʈ Ŭ�������� ����

	bool bBroken;
};

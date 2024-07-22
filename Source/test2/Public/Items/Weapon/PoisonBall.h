// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PoisonBall.generated.h"

class USphereComponent;

UCLASS()
class TEST2_API APoisonBall : public AActor
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;

	UFUNCTION()	//
	void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	void ExecuteGetHitEnemy(AActor* BoxHit);
	void SpawnHitParticles(const FVector& ImpactPoint);

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	USphereComponent* WeaponSphere;	// 무기의 공격 판정을 담당할 콜라이더

	UPROPERTY(EditAnywhere, Category = "Combat")
	UParticleSystem* HitParticles;

public:	
	APoisonBall();

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* PoisonBallMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UProjectileMovementComponent* ProjectileMovementComponent;

	void LaunchFireball(const FVector& LaunchVelocity);
private:
	void SetDead();
};

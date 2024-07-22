// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FireStorm.generated.h"

class UCapsuleComponent;

UCLASS()
class TEST2_API AFireStorm : public AActor
{
	GENERATED_BODY()
protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnCapsuleOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	void ExecuteGetHitEnemy(AActor* BoxHit);
	void SpawnHitParticles(const FVector& ImpactPoint);

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	UCapsuleComponent* WeaponSphere;

	UPROPERTY(EditAnywhere, Category = "Combat")
	UParticleSystem* HitParticles;

public:
	AFireStorm();

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* FireStormMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* OutDistMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* InDistMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UProjectileMovementComponent* ProjectileMovementComponent;
	void HideMesh();
private:
	void SetDead(float DeltaTime);
	void Spawning(float DeltaTime);

	FVector  GetRandDiretion();
	void RandMovement(float DeltaTime);

	float SpawnTime = 0.f;
	float InDistScale = 10.f;
	bool CheckLifeTime = false;
	float LifeTime = 0.f;

	float MoveSpeed = 400.f;
	FVector MovementDirection;
	float ResetDirectionTime = 0.f;
};

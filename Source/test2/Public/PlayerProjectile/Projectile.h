// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Weapon/Weapon.h"
#include "Projectile.generated.h"

/**
 * 
 */

class UProjectileMovementComponent;
class USkeletalMeshSocket;
class UParticleComponent;
class UNiagaraComponent;

UCLASS()
class TEST2_API AProjectile : public AWeapon
{
	GENERATED_BODY()
	
public:
	AProjectile();
	void Shoot();

protected:
	virtual void BeginPlay() override;
	void OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	void PlayExplosionParticle();

private:
	UPROPERTY(EditDefaultsOnly)
	TArray<UStaticMesh*> MeshVector;

	UPROPERTY(EditDefaultsOnly)
	UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(EditDefaultsOnly, Category = "Particle")
	UParticleSystemComponent* ProjectileBodyParticle;

	UPROPERTY(EditDefaultsOnly, Category = "Particle")
	UNiagaraComponent* ProjectileExplosionParticle;

	FTimerHandle AutoExplosionTimer;
};
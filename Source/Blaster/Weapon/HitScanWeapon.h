// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "HitScanWeapon.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API AHitScanWeapon : public AWeapon
{
	GENERATED_BODY()
	
public:

	virtual void Fire(const FVector& HitTarget) override;

protected:

	FVector TraceEndWithScatter(const FVector& TraceStart, const FVector& HitTarget);

	void WeaponTraceHit(const FVector& TraceStart, const FVector& HitTarget, FHitResult& OutHit);

	UPROPERTY(EditAnywhere, Category = "Damage")
	UParticleSystem* BeamParticles;

	UPROPERTY(EditAnywhere, Category = "Damage")
	USoundCue* ImpactSound;

	UPROPERTY(EditAnywhere, Category = "Damage")
	class UParticleSystem* ImpactParticles;

	UPROPERTY(EditAnywhere, Category = "Damage")
	float Damage = 7.f;

private:

	UPROPERTY(EditAnywhere, Category = "Damage")
	UParticleSystem* MuzzleFlash;
	
	UPROPERTY(EditAnywhere, Category = "Damage")
	USoundCue* FireSound;
	

	//**
	// Trace end with scatter*//

	UPROPERTY(EditAnywhere, Category = "Weapon Scatter")
	float DistanceToSphere = 800.f;	
	
	UPROPERTY(EditAnywhere, Category = "Weapon Scatter")
	float SphereRadius = 75.f;

	UPROPERTY(EditAnywhere, Category = "Weapon Scatter")
	bool bUseScatter = false;

	};

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



private:

	UPROPERTY(EditAnywhere, Category = "Damage")
	class UParticleSystem* ImpactParticles;

	UPROPERTY(EditAnywhere, Category = "Damage")
	float Damage = 7.f;
	
	UPROPERTY(EditAnywhere, Category = "Damage")
	UParticleSystem* BeamParticles;

	UPROPERTY(EditAnywhere, Category = "Damage")
	UParticleSystem* MuzzleFlash;
	
	UPROPERTY(EditAnywhere, Category = "Damage")
	USoundCue* FireSound;
	
	UPROPERTY(EditAnywhere, Category = "Damage")
	USoundCue* ImpactSound;
};

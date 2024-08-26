// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

UCLASS()
class BLASTER_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	AProjectile();
	virtual void Tick(float DeltaTime) override;

	virtual void Destroyed() override;

	UPROPERTY(EditAnywhere)
	class UBoxComponent* CollisionBox;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UPROPERTY(EditAnywhere, Category = "Damage")
	float Damage = 20.f;



	UPROPERTY(EditAnywhere)
	class UParticleSystem* ImpactParticles;

	UPROPERTY(EditAnywhere)
	class UParticleSystem* PlayerImpactParticles;

	UPROPERTY()
	class UParticleSystem* ParticlesToPlay;

	UPROPERTY(EditAnywhere)
	class USoundCue* ImpactSound;

	UPROPERTY(EditAnywhere)
	class USoundCue* PlayerImpactSound;

private:

	UPROPERTY(VisibleAnywhere)
	class UProjectileMovementComponent* ProjectileMovementComponent;

	UPROPERTY(EditAnywhere)
	class UParticleSystem* Tracer;

	UPROPERTY()
	class UParticleSystemComponent* TracerComponent;

public:	

};

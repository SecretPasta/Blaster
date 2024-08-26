// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "ProjectileRocket.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API AProjectileRocket : public AProjectile
{
	GENERATED_BODY()
	
public:

	AProjectileRocket();

	virtual void Destroyed() override;

protected:

	virtual void BeginPlay() override;

	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;

	UPROPERTY(EditAnywhere, Category = "Damage")
	class UNiagaraSystem* TrailSystem;

	void DestroyTimerFinished();

	UPROPERTY()
	class UNiagaraComponent* TrailSystemComponent;

	UPROPERTY(EditAnywhere, Category = "Damage")
	class USoundCue* ProjectileLoop;

	UPROPERTY()
	class UAudioComponent* ProjecileLoopComponent;

	UPROPERTY(EditAnywhere, Category = "Damage")
	USoundAttenuation* LoopingSoundAttenuation;

private:

	UPROPERTY(VisibleAnywhere, Category = "Damage")
	UStaticMeshComponent* RocketMesh;

	UPROPERTY(EditAnywhere, Category = "Damage")
	float MinimumDamage = 10.f;

	UPROPERTY(EditAnywhere, Category = "Damage")
	float InnerRadiusDamage = 200.f;

	UPROPERTY(EditAnywhere, Category = "Damage")
	float OuterRadiusDamage = 500.f;

	UPROPERTY(EditAnywhere, Category = "Damage")
	float DamageFalloff = 1.f;

	FTimerHandle DestroyTimer;
	
	UPROPERTY(EditAnywhere, Category = "Damage")
	float DestroyTime = 3.f;

};

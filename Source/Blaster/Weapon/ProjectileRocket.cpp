// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileRocket.h"
#include "Kismet/GameplayStatics.h"
#include "Components/StaticMeshComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Sound/SoundCue.h"
#include "Components/BoxComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraSystemInstance.h"
#include "Components/AudioComponent.h"




AProjectileRocket::AProjectileRocket()
{
	RocketMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RocketMesh"));
	RocketMesh->SetupAttachment(RootComponent);
	RocketMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

}


void AProjectileRocket::BeginPlay()
{
	Super::BeginPlay();

	if (!HasAuthority()) {
		CollisionBox->OnComponentHit.AddDynamic(this, &AProjectileRocket::OnHit);
	}

	if (TrailSystem) {
		TrailSystemComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
			TrailSystem, 
			GetRootComponent(), 
			FName(),
			GetActorLocation(),
			GetActorRotation(),
			EAttachLocation::KeepWorldPosition,
			false
		);
	}

	if (ProjectileLoop && LoopingSoundAttenuation) {
		ProjecileLoopComponent = UGameplayStatics::SpawnSoundAttached(
			ProjectileLoop, 
			GetRootComponent(), 
			FName(), 
			GetActorLocation(), 
			EAttachLocation::KeepWorldPosition,
			false,
			1.f,
			1.f,
			0.f,
			LoopingSoundAttenuation,
			(USoundConcurrency*)nullptr,
			false
		);
	}

}

void AProjectileRocket::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{

	APawn* FiringPawn = GetInstigator();
	if (FiringPawn && HasAuthority()) {
		AController* FiringController = FiringPawn->GetController();
		if (FiringController) {
			UGameplayStatics::ApplyRadialDamageWithFalloff(
				this,
				Damage, //Base Damage
				MinimumDamage, //Minimum Damge
				GetActorLocation(), // Origin
				InnerRadiusDamage, //Damage Inner Radius
				OuterRadiusDamage, // DamageOuterRadius
				DamageFalloff, // DamageFallOff
				UDamageType::StaticClass(), //Damage Class Type
				TArray<AActor*>(), //Ignore all Actors
				this,// Damage Cause
				FiringController //Instigator Controller
			);
		}
	}

	GetWorldTimerManager().SetTimer(DestroyTimer, this, &AProjectileRocket::DestroyTimerFinished, DestroyTime);

	if (PlayerImpactParticles) {
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), PlayerImpactParticles, GetActorTransform());
	}
	if (ImpactSound) {
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), ImpactSound, GetActorLocation());
	}
	if (RocketMesh) {
		RocketMesh->SetVisibility(false);
	}
	if (CollisionBox) {
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	if (TrailSystemComponent && TrailSystemComponent->GetSystemInstance()) {
		TrailSystemComponent->GetSystemInstance()->Deactivate();
	}
	if (ProjecileLoopComponent && ProjecileLoopComponent->IsPlaying()) {
		ProjecileLoopComponent->Stop();
	}
	//Super::OnHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);
}

void AProjectileRocket::DestroyTimerFinished()
{
	Destroy();
}

void AProjectileRocket::Destroyed()
{

}

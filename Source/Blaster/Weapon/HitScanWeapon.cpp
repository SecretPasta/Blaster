// Fill out your copyright notice in the Description page of Project Settings.


#include "HitScanWeapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"

void AHitScanWeapon::Fire(const FVector& HitTarget)
{
	Super::Fire(HitTarget);

	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (OwnerPawn == nullptr) {
		return;
	}
	AController* InstigatorController = OwnerPawn->GetController();

	const USkeletalMeshSocket* MuzzleFlashSocket =  GetWeaponMesh()->GetSocketByName("MuzzleFlash");


	if (MuzzleFlashSocket == nullptr) {
		return;
	}

	FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
	FVector Start = SocketTransform.GetLocation();
	FVector End = Start + (HitTarget - Start) * 1.25f;

	FHitResult FireHit;

	UWorld* World = GetWorld();

	if (World == nullptr) {
		return;
	}
	World->LineTraceSingleByChannel(FireHit, Start, End, ECollisionChannel::ECC_Visibility);

	FVector BeamEnd;

	if (!FireHit.bBlockingHit) {
		BeamEnd = End;
		if (BeamParticles) {
			UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(World, BeamParticles, SocketTransform);
			if (Beam) {
				Beam->SetVectorParameter(FName("Target"), BeamEnd);
			}
		}
		return;
	}
	BeamEnd = FireHit.ImpactPoint;
	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(FireHit.GetActor());
	if (BlasterCharacter) {
		if (HasAuthority()) {
			UGameplayStatics::ApplyDamage(BlasterCharacter, Damage, InstigatorController, this, UDamageType::StaticClass());

		}
	}
	if (ImpactParticles) {
		UGameplayStatics::SpawnEmitterAtLocation(World, ImpactParticles, FireHit.ImpactPoint, FireHit.ImpactNormal.Rotation());
	}
	if (BeamParticles) {
		UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(World, BeamParticles, SocketTransform);
		if (Beam) {
			Beam->SetVectorParameter(FName("Target"), BeamEnd);
		}
	}

	if (MuzzleFlash) {
		UGameplayStatics::SpawnEmitterAtLocation(World, MuzzleFlash, SocketTransform);
	}
	if (FireSound) {
		UGameplayStatics::PlaySoundAtLocation(World, FireSound, GetActorLocation());
	}
	if (ImpactSound) {
		UGameplayStatics::PlaySoundAtLocation(World, ImpactSound, FireHit.ImpactPoint);
	}

}

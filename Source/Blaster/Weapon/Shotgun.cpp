// Fill out your copyright notice in the Description page of Project Settings.


#include "Shotgun.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"
#include "Kismet/KismetMathLibrary.h"
#include "Blaster/PlayerController/BlasterPlayerController.h"
#include "Blaster/BlasterComponents/LagCompensationComponent.h"

void AShotgun::FireShotgun(const TArray<FVector_NetQuantize>& HitTargets)
{
	AWeapon::Fire(FVector());
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (OwnerPawn == nullptr) {
		return;
	}
	AController* InstigatorController = OwnerPawn->GetController();
	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName("MuzzleFlash");

	if (MuzzleFlashSocket == nullptr) {
		return;
	}
	const FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
	const FVector Start = SocketTransform.GetLocation();
	//Maps hit character to number of times hit
	TMap<ABlasterCharacter*, uint32> HitMap;
	TMap<ABlasterCharacter*, uint32> HeadShotHitMap;
	for (FVector_NetQuantize HitTarget : HitTargets) {
		FHitResult FireHit;
		WeaponTraceHit(Start, HitTarget, FireHit);
		ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(FireHit.GetActor());
		if (BlasterCharacter)
		{
			const bool bHeadshot = FireHit.BoneName.ToString() == FString("head");;
			
			if (bHeadshot)
			{
				if (HeadShotHitMap.Contains(BlasterCharacter)) HitMap[BlasterCharacter]++;
				else HeadShotHitMap.Emplace(BlasterCharacter, 1);
			}
			else
			{
				if (HitMap.Contains(BlasterCharacter)) HitMap[BlasterCharacter]++;
				else HitMap.Emplace(BlasterCharacter, 1);
			}
		}

		if (ImpactParticles)
		{
			UGameplayStatics::SpawnEmitterAtLocation(
				GetWorld(),
				ImpactParticles,
				FireHit.ImpactPoint,
				FireHit.ImpactNormal.Rotation()
			);
		}
		if (ImpactSound)
		{
			UGameplayStatics::PlaySoundAtLocation(
				this,
				ImpactSound,
				FireHit.ImpactPoint,
				.5f,
				FMath::FRandRange(-.5f, .5f)
			);
		}
	}
	TArray<ABlasterCharacter*> HitCharacters;

	//Maps character hit to total damge
	TMap<ABlasterCharacter*, float> DamageMap;

	//Calculate body shot damage by multiplying times hit X damage - store in damageMap
	for (auto& HitPair : HitMap)
	{
		if (HitPair.Key)
		{
			DamageMap.Emplace(HitPair.Key, HitPair.Value * Damage);
			HitCharacters.AddUnique(HitPair.Key);
		}
	}
	//Calculate headshot shot damage by multiplying times hit X headshot damage - store in damageMap
	for (auto& HeadShotHitPair : HeadShotHitMap)
	{
		if (HeadShotHitPair.Key)
		{
			if (DamageMap.Contains(HeadShotHitPair.Key)) DamageMap[HeadShotHitPair.Key] += HeadShotHitPair.Value * HeadShotDamage;
			else DamageMap.Emplace(HeadShotHitPair.Key, HeadShotHitPair.Value * HeadShotDamage);
			HitCharacters.AddUnique(HeadShotHitPair.Key);
		}
	}

	//Loop through damage map to get total damage to apply
	for (auto DamagePair : DamageMap)
	{
		if (DamagePair.Key && InstigatorController)
		{
			bool bCauseAuthDamage = !bUseServerSideRewind || OwnerPawn->IsLocallyControlled();
			if (HasAuthority() && bCauseAuthDamage)
				UGameplayStatics::ApplyDamage(
					DamagePair.Key, //Character thas was hit
					DamagePair.Value, // Damage calculated at the for loops above
					InstigatorController,
					this,
					UDamageType::StaticClass()
				);
		}
	}


	if (!HasAuthority() && bUseServerSideRewind) {
		BlasterOwnerCharacter = BlasterOwnerCharacter == nullptr ? Cast<ABlasterCharacter>(OwnerPawn) : BlasterOwnerCharacter;
		BlasterOwnerController = BlasterOwnerController == nullptr ? Cast<ABlasterPlayerController>(InstigatorController) : BlasterOwnerController;
		if (BlasterOwnerController && BlasterOwnerCharacter && BlasterOwnerCharacter->GetLagCompensation() && BlasterOwnerCharacter->IsLocallyControlled()) {
			BlasterOwnerCharacter->GetLagCompensation()->ShotgunServerScoreRequest(
				HitCharacters, 
				Start, 
				HitTargets, 
				BlasterOwnerController->GetServerTime() - BlasterOwnerController->SingleTripTime);
		}
	}
}

void AShotgun::ShotgunTraceEndWithScatter(const FVector& HitTarget, TArray<FVector_NetQuantize>& HitTargets)
{
	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName("MuzzleFlash");
	if (MuzzleFlashSocket == nullptr) {
		return;
	}
	const FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
	const FVector TraceStart = SocketTransform.GetLocation();
	const FVector ToTargetNormalized = (HitTarget - TraceStart).GetSafeNormal();
	const FVector SphereCenter = TraceStart + ToTargetNormalized * DistanceToSphere;

	for (uint32 i = 0; i < NumberOfPellets; ++i) {
		const FVector RandVec = UKismetMathLibrary::RandomUnitVector() * FMath::FRandRange(0.f, SphereRadius);
		const FVector EndLoc = SphereCenter + RandVec;
		const FVector ToEndLoc = EndLoc - TraceStart;
		HitTargets.Emplace(TraceStart + ToEndLoc * TRACE_LENGTH / ToEndLoc.Size());
	}
}

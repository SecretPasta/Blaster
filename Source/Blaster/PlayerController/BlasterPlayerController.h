// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Blaster/Weapon/WeaponTypes.h"
#include "BlasterPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API ABlasterPlayerController : public APlayerController
{
	GENERATED_BODY()

public:

	void SetHUDHealth(float Health, float MaxHealth);

	void SetHUDScore(float Score);

	void SetHUDDefeats(int32 Defeats);

	void SetHUDWeaponAmmo(int32 Ammo);

	void SetHUDCarriedAmmo(int32 Ammo);

	void SetHUDWeaponType(EWeaponType WeaponType);

	void SetHUDMatchCountdown(float CountdownTime);

	virtual void OnPossess(APawn* InPawn) override;

	virtual void Tick(float DeltaTime) override;

	virtual float GetServerTime();//Synced with server world clock

	virtual void ReceivedPlayer() override;//Synced with server clock as soon as possible
protected:

	virtual void BeginPlay() override;

	void SetHUDTime();

	/**
	*Sync Time between client and server
	**/

	//Requests the current server time passing in the client's time when the request was sent
	UFUNCTION(Server, Reliable)
	void ServerRequestServerTime(float TimeOfClientRequest);

	//Reports the current server time to the client in response to ServerRequestServerTime
	UFUNCTION(Server, Reliable)
	void ClientReportServerTime(float TimeClientRequest, float TimeServerReceivedClientRequest);

	//Differance between client and server time
	float ClientServerDelta = 0.f;

	UPROPERTY(EditAnywhere, Category = "Time")
	float TimeSyncFrequency = 5.f;

	float TimeSyncRunningTime = 0.f;

	void CheckTimeSync(float DeltaTime);
private:

	UPROPERTY()
	class ABlasterHUD* BlasterHUD;
	
	float MatchTime = 120.f;

	uint32 CountdownInt = 0;

};

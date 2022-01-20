// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "FPSAIGuard.generated.h"

class UPawnSensingComponent;
class AAIController;
class ATargetPoint;


// Any Enums exposed to blueprints must be a child of uint8

UENUM(BlueprintType)
enum class EAIState : uint8
{
	EAIS_Idle UMETA(Display = "Idle"),
	EAIS_Alerted UMETA(Display = "Alerted"),
	EAIS_Investigating UMETA(Display = "Investigating"),
	EAIS_Max UMETA(Display="DefaultMax")
};

UCLASS()
class FPSGAME_API AFPSAIGuard : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AFPSAIGuard();

protected:

	UFUNCTION(BlueprintImplementableEvent, Category="AI")
	void OnStateChanged(EAIState NewState);

	void SetGuardState(EAIState NewState);

	UFUNCTION()
	void ResetOrientation();

	UFUNCTION()
	void PatrolToLocation();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category="Components")
	UPawnSensingComponent* PawnSensingComp;

	FRotator OriginalRotation;

	FTimerHandle ResetGuardViewTimer;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="AI")
	float ResetViewSecs;

	UPROPERTY(ReplicatedUsing=OnRep_GuardState)
	EAIState GuardState;

	// only called on the clients
	UFUNCTION()
	void OnRep_GuardState();

	AAIController* AIController;

	FTimerHandle ResetPatrolTimerHandle;

	UPROPERTY(EditDefaultsOnly, Category="AI")
	TSubclassOf<ATargetPoint> TargetPointClass;

	AActor* TargetPoint1;

	AActor* TargetPoint2;

	AActor* CurrentTargetPoint;

	UPROPERTY(EditAnywhere, Category = "AI")
	bool bPatrol;

	bool bHasReachedTarget1;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/*
	* Remember that as soon as OnSeePawn delegate is called, then OnHearNoise delegate is no longer called.
	* In other words, you wont trigger a "I heard a noise" function while the "I see something" condition is met.	
	*/

	UFUNCTION()
	void OnPawnSeen(APawn* SeenPawn);

	UFUNCTION()
	void OnNoiseHeard(APawn* InstigatingPawn, const FVector& Location, float Volume);

};

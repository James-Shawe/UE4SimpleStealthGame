// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LaunchPad.generated.h"

class UBoxComponent;

UCLASS()
class FPSGAME_API ALaunchPad : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALaunchPad();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LaunchPadProperties")
	float LaunchMagnitude;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LaunchPadProperties")
	float LaunchAngle;

	UFUNCTION()
	void TriggerBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

protected:

	UPROPERTY(VisibleAnywhere, Category="Components")
	UStaticMeshComponent* LaunchPadMesh;

	UPROPERTY(VisibleAnywhere, Category="Components")
	UBoxComponent* TriggerBox;

	UPROPERTY(EditDefaultsOnly, Category = "FX")
	UParticleSystem* LaunchParticles;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

};

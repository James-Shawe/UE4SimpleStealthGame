// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Blackhole.generated.h"

class USphereComponent;

UCLASS()
class FPSGAME_API ABlackhole : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABlackhole();

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	UStaticMeshComponent* BlackHoleMesh;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	USphereComponent* DestroySphere;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
	float ForceStrength;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
	float CollisionSphereRadius;

	TArray<FHitResult> OutHits;

	FCollisionShape CollisionSphere;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void DestroySphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};

// Fill out your copyright notice in the Description page of Project Settings.


#include "Blackhole.h"
#include "Components/SphereComponent.h"
#include "FPSCharacter.h"

// Sets default values
ABlackhole::ABlackhole()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CollisionSphereRadius = 10000.f;
	ForceStrength = -8000.f;

	// Create the mesh component for the blackhole and disable collisions to all channnels
	BlackHoleMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	BlackHoleMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BlackHoleMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	RootComponent = BlackHoleMesh;

	// Used to check which overlapped actors shall have radial force applied to them
	// This is the "Event Horizon"
	CollisionSphere = FCollisionShape::MakeSphere(CollisionSphereRadius);

	// This is inside the mesh and destroys the objects that overlap it.
	// Create the inner sphere and set collision to query only and to generate overlap events for all channels.
	DestroySphere = CreateDefaultSubobject<USphereComponent>(TEXT("Destroy Sphere"));
	DestroySphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	DestroySphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	DestroySphere->SetSphereRadius(100.f);
	DestroySphere->SetupAttachment(GetRootComponent());

}

// Called when the game starts or when spawned
void ABlackhole::BeginPlay()
{
	Super::BeginPlay();

	DestroySphere->OnComponentBeginOverlap.AddDynamic(this, &ABlackhole::DestroySphereOnOverlapBegin);
	
}

// Called every frame
void ABlackhole::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector Location = GetActorLocation();

	bool isHit = GetWorld()->SweepMultiByChannel(OutHits, Location, Location, FQuat::Identity, ECollisionChannel::ECC_WorldDynamic, CollisionSphere);

	if (isHit)
	{
		for (auto& Hit : OutHits)
		{
			if (Hit.GetActor())
			{
				UStaticMeshComponent* OverlappedActorsMesh = Cast<UStaticMeshComponent>(Hit.GetActor()->GetRootComponent());

				if (OverlappedActorsMesh)
				{
					OverlappedActorsMesh->AddRadialForce(GetActorLocation(), CollisionSphereRadius, ForceStrength, ERadialImpulseFalloff::RIF_Constant, true);
				}
			}
		}
	}

}

void ABlackhole::DestroySphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AFPSCharacter* Character = Cast<AFPSCharacter>(OtherActor);

	if (Character)
	{
		return;
	}
	else
	{
		OtherActor->Destroy();
	}
}
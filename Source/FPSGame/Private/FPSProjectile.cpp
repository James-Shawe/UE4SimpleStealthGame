// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "FPSProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"

AFPSProjectile::AFPSProjectile() 
{
	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &AFPSProjectile::OnHit);	// set up a notification for when this component hits something blocking

	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	// Set as root component
	RootComponent = CollisionComp;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;

	// Die after 3 seconds by default
	InitialLifeSpan = 3.0f;

	/*Server always tells client what to do. Client can never tell server what to do. It can only listen.
	* For this Actor, this means the fire method (in FPSCharacter.cpp) must only be called the server, never by the client.
	* If the client does use the fire method, then a projectile actor will be spawned locally.
	* i.e Server Authorative
	*/

	SetReplicates(true); //Allows UE4 to send requests to clients to copy what happens on the server (Actor spawns etc)
	SetReplicateMovement(true); //Location, Rotation etc is also updated on clients when it changes on the server

}


void AFPSProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Only add impulse and destroy projectile if we hit a physics
	if ((OtherActor != NULL) && (OtherActor != this) && (OtherComp != NULL) && OtherComp->IsSimulatingPhysics())
	{
		OtherComp->AddImpulseAtLocation(GetVelocity() * 100.0f, GetActorLocation());
	}

	//Instigator is a member var of every actor, usually used in damage functions and calculations
	// It is important that we pass that in here, as it will look up if the pawn referenced by instigator 
	// has a NoiseEmitterComponent.


	// Check if the machine calling this is the server. Only want Clients to simulate what is happening on the server.
	if (GetLocalRole() == ROLE_Authority)
	{
		MakeNoise(1.f, GetInstigator());
		Destroy();
	}	
}
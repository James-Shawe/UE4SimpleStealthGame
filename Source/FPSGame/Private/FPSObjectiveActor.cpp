// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSObjectiveActor.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "FPSCharacter.h"

// Sets default values
AFPSObjectiveActor::AFPSObjectiveActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;


	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Component"));
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RootComponent = MeshComp;

	/*Create Sphere component and set the collison to Spatial queries only. This saves resources by not including it in physics simulations.
	* Then Set the collision response to channels to ignore, so it wont block anything.
	* Then set the collision response to the pawn channel to report a overlap when the pawn collides with it.
	*/

	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Component"));
	SphereComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereComp->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	SphereComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	SphereComp->SetupAttachment(GetRootComponent());

	SetReplicates(true);
}

// Called when the game starts or when spawned
void AFPSObjectiveActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void AFPSObjectiveActor::PlayEffects()
{
	/*UGameplayStatics::SpawnEmitterAtLocation(this, PickupFX, GetActorLocation()),
	* Will also work, since passing the ObjectiveActor will make Unreal figure out the world it is in.
	*/
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), PickupFX, GetActorLocation());
}

/* This is different from using delegates and OnComponentOverlapBegin().AddDynamic(this, &FunctionName),
* This way acts as a catch all, as it will trigger regardless of whether is the meshcomponent or spherecomponent
* that collides with the other actor.
*/

void AFPSObjectiveActor::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	PlayEffects(); // will play on local and server

	if (GetLocalRole() == ROLE_Authority) // will only run if its the server
	{
		AFPSCharacter* MyCharacter = Cast<AFPSCharacter>(OtherActor);

		if (MyCharacter)
		{
			MyCharacter->bIsCarryingObjective = true; // Since this var is replicated, when the server sets this, all clients are updated as well.

			Destroy(); // gives the appearance that the objective has been picked up
		}
	}
}
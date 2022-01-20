// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSExtractionZone.h"
#include "Components/BoxComponent.h"
#include "Components/DecalComponent.h"
#include "FPSCharacter.h"
#include "FPSGameMode.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AFPSExtractionZone::AFPSExtractionZone()
{
	ExtractionZoneSize = FVector(200.f);

	OverlapComp = CreateDefaultSubobject<UBoxComponent>(TEXT("OverlapComp"));
	OverlapComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	OverlapComp->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	OverlapComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	OverlapComp->SetBoxExtent(ExtractionZoneSize);
	RootComponent = OverlapComp;


	DecalComp = CreateDefaultSubobject<UDecalComponent>(TEXT("DecalComp"));
	DecalComp->DecalSize = ExtractionZoneSize;
	DecalComp->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void AFPSExtractionZone::BeginPlay()
{
	Super::BeginPlay();

	OverlapComp->OnComponentBeginOverlap.AddDynamic(this, &AFPSExtractionZone::OverlapCompBeginOverlap);
	
}

void AFPSExtractionZone::OverlapCompBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AFPSCharacter* PlayerCharacter = Cast<AFPSCharacter>(OtherActor);

	if (!PlayerCharacter)
	{
		return;
	}

	if (PlayerCharacter->bIsCarryingObjective)
	{
		// GetAuthGameMode() will return nullptr on a client in a multiplayer game
		// This check would have to be on the server side. For singleplayer it will work.
		AFPSGameMode* GameMode = Cast<AFPSGameMode>(GetWorld()->GetAuthGameMode());

		if (GameMode)
		{
			GameMode->CompleteMission(PlayerCharacter, true);
		}
	}
	else
	{
		UGameplayStatics::PlaySound2D(GetWorld(), ObjectiveMissingSoundCue);
	}
}
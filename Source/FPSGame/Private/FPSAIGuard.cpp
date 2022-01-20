// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSAIGuard.h"
#include "Perception/PawnSensingComponent.h"
#include "DrawDebugHelpers.h"
#include "FPSGameMode.h"
#include "AIController.h"
#include "Engine/TargetPoint.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AFPSAIGuard::AFPSAIGuard()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComp"));

	ResetViewSecs = 3.f;

	GuardState = EAIState::EAIS_Idle;

	bHasReachedTarget1 = false;
	bPatrol = false;
}

// Called when the game starts or when spawned
void AFPSAIGuard::BeginPlay()
{
	Super::BeginPlay();

	AIController = Cast<AAIController>(GetController());

	OriginalRotation = GetActorRotation();

	PawnSensingComp->OnSeePawn.AddDynamic(this, &AFPSAIGuard::OnPawnSeen);
	PawnSensingComp->OnHearNoise.AddDynamic(this, &AFPSAIGuard::OnNoiseHeard);

	/* Find all target actors in the level and assign first two to the patrol route*/

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), TargetPointClass, FoundActors);

	TargetPoint1 = FoundActors[0];
	TargetPoint2 = FoundActors[1];
	CurrentTargetPoint = nullptr;

	if (bPatrol)
	{
		GetWorldTimerManager().SetTimer(ResetPatrolTimerHandle, this, &AFPSAIGuard::PatrolToLocation, 3.f);
	}
}

// Called every frame
void AFPSAIGuard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (((GuardState == EAIState::EAIS_Investigating) || (GuardState == EAIState::EAIS_Alerted)) && bPatrol && AIController)
	{
		AIController->StopMovement();
	}

	/*If the guard is patrolling then stop when within 50 units of the selected TargetPoint*/

	if (CurrentTargetPoint)
	{
		FVector DeltaDistance = GetActorLocation() - CurrentTargetPoint->GetActorLocation();
		float DistanceToTarget = DeltaDistance.Size();

		if (DistanceToTarget < 100.f)
		{
			UE_LOG(LogTemp, Warning, TEXT("within 100 units"));
			if (TargetPoint1 == CurrentTargetPoint)
			{
				bHasReachedTarget1 = true;
			}
			else
			{
				bHasReachedTarget1 = false;

			}
			PatrolToLocation();
		}
	}

}

// When variable EAIState is updated this function will be called on the clients
void AFPSAIGuard::OnRep_GuardState()
{
	OnStateChanged(GuardState);
}

void AFPSAIGuard::SetGuardState(EAIState NewState)
{
	if (GuardState == NewState) return;

	GuardState = NewState;
	OnRep_GuardState(); // so that if the host player is also playing there is no difference to playing on the server or client
}

void AFPSAIGuard::OnPawnSeen(APawn* SeenPawn)
{
	if (!SeenPawn)
	{
		return;
	}

	// GetAuthGameMode() will return nullptr on a client in a multiplayer game
		// This check would have to be on the server side. For singleplayer it will work.
	AFPSGameMode* GameMode = Cast<AFPSGameMode>(GetWorld()->GetAuthGameMode());

	if (GameMode)
	{
		GameMode->CompleteMission(SeenPawn, false);
	}

	SetGuardState(EAIState::EAIS_Alerted);

	DrawDebugSphere(GetWorld(), SeenPawn->GetActorLocation(), 32.f, 12, FColor::Blue, false, 10.f);
}

void AFPSAIGuard::OnNoiseHeard(APawn* NoiseInstigator, const FVector& Location, float Volume)
{
	if (GuardState == EAIState::EAIS_Alerted) return;

	DrawDebugSphere(GetWorld(), Location, 32.f, 12, FColor::Green, false, 10.f);

	// Location is unit vector

	FVector Direction = Location - GetActorLocation();
	Direction.Normalize(); // create a direction vector

	FRotator DirectionOfNoise = GetActorRotation();

	DirectionOfNoise.Yaw = FRotationMatrix::MakeFromX(Direction).Rotator().Yaw; //Make a rotator from the x axis of a vector.
	SetActorRotation(DirectionOfNoise);

	GetWorldTimerManager().ClearTimer(ResetGuardViewTimer); // clear any previously running timer
	GetWorldTimerManager().SetTimer(ResetGuardViewTimer, this, &AFPSAIGuard::ResetOrientation, ResetViewSecs);

	SetGuardState(EAIState::EAIS_Investigating);
}

void AFPSAIGuard::ResetOrientation()
{
	if (GuardState == EAIState::EAIS_Alerted) return;

	SetActorRotation(OriginalRotation);

	if (bPatrol)
	{
		PatrolToLocation();
	}
	
	SetGuardState(EAIState::EAIS_Idle);
}

void AFPSAIGuard::PatrolToLocation()
{
	if (!AIController)
	{
		UE_LOG(LogTemp, Warning, TEXT("No AI Controller Set"));
		return;
	}

	if (bHasReachedTarget1)
	{
		AIController->MoveToLocation(TargetPoint2->GetActorLocation(), 0.f);
		CurrentTargetPoint = TargetPoint2;
	}
	else
	{
		AIController->MoveToLocation(TargetPoint1->GetActorLocation(), 0.f);
		CurrentTargetPoint = TargetPoint1;
	}
}

/* You need to use this function (Inherited from character.h) in conjunction with the UPROPERTY(ReplicatedUsing=)*/

void AFPSAIGuard::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFPSAIGuard, GuardState); // This tells UE4 to replicate for all clients
}
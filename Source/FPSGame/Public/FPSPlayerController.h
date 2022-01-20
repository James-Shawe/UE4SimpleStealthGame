// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "FPSPlayerController.generated.h"

/**
 * PlayerController exists on both the server and on the client that owns this PlayerController.
 * Pawns Exist everywhere. For two players, there will be two player controllers on the server, and each client
 * will only have one player controller. Both Server and Clients will all have their own copies of pawns.
 */
UCLASS()
class FPSGAME_API AFPSPlayerController : public APlayerController
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintImplementableEvent, Category="PlayerController")
	void  OnMissionCompleted(APawn* InstigatorPawn, bool bMissionSuccess);
	
};

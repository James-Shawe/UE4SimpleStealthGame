// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "FPSGameState.generated.h"

/**
 * GameState works with the GameMode to bridge the comms between the gamemode that is server only and the clients
 */
UCLASS()
class FPSGAME_API AFPSGameState : public AGameStateBase
{
	GENERATED_BODY()

public:

	/*NetMulticast means that if this were to be called from the server, then it would be sent to all
	* clients and will run on all clients including the server.
	*/

	UFUNCTION(NetMulticast, Reliable)
	void MulticastOnMissionComplete(APawn* InstigatorPawn, bool bMissionSuccess);
	
};

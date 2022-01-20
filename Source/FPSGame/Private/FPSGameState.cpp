// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSGameState.h"
#include "FPSPlayerController.h"

void AFPSGameState::MulticastOnMissionComplete_Implementation(APawn* InstigatorPawn, bool bMissionSuccess)
{
	/* Iterate through all the player controllers in the world. */
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; It++)
	{
		AFPSPlayerController* PlayerController = Cast<AFPSPlayerController>(It->Get());

		if (PlayerController && PlayerController->IsLocalController()) // only display the HUD messages for the Client that owns the controller.
		{
			PlayerController->OnMissionCompleted(InstigatorPawn, bMissionSuccess); // Creates Gameover widget blueprint in blueprints.

			// Disable Input to Pawns
			APawn* PlayerPawn = PlayerController->GetPawn();
			if (PlayerPawn)
			{
				PlayerPawn->DisableInput(PlayerController);
			}
		}
	}
}
// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/BSGameStateBase.h"

#include "Kismet/GameplayStatics.h"
#include "Player/BSPlayerController.h"
#include "EngineUtils.h"
//#include "Net/UnrealNetwork.h"



void ABSGameStateBase::MulticastRPCBroadcastLoginMessage_Implementation(const FString& InNameString)
{
	if (HasAuthority() == false)
	{
		APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		if (IsValid(PC) == true)
		{
			ABSPlayerController* BSPC = Cast<ABSPlayerController>(PC);
			if (IsValid(BSPC) == true)
			{
				FString NotificationString = InNameString + TEXT(" has joined the game.");
				BSPC->PrintChatMessageString(NotificationString);
			}
		}
	}
}

//void ABSGameStateBase::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
//{
//	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
//
//	DOREPLIFETIME(ThisClass, RemainingTurnTime);
//	DOREPLIFETIME(ThisClass, bIsTurnActive);
//	
//}



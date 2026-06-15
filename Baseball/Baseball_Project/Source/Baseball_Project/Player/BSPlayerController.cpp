// Fill out your copyright notice in the Description page of Project Settings.


#include "BSPlayerController.h"

#include "UI/BSChatInput.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Baseball_Project/Baseball_Project.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "Game/BSGameModeBase.h"
#include "BSPlayerState.h"
#include "Net/UnrealNetwork.h"

ABSPlayerController::ABSPlayerController()
{
	bReplicates = true;
}

void ABSPlayerController::BeginPlay()
{

	Super::BeginPlay();

	if (IsLocalController() == false)
	{
		return;
	}

	FInputModeUIOnly InputModeUIOnly;
	SetInputMode(InputModeUIOnly);

	if (IsValid(ChatInputWidgetClass) == true)
	{
		
		ChatInputWidgetInstance = CreateWidget<UBSChatInput>(this, ChatInputWidgetClass);
		if (IsValid(ChatInputWidgetInstance) == true)
		{
			
			ChatInputWidgetInstance->AddToViewport();
		}
	}

	if (IsValid(NotificationTextWidgetClass) == true)
	{
		NotificationTextWidgetInstance = CreateWidget<UUserWidget>(this, NotificationTextWidgetClass);
		
		if (IsValid(ChatInputWidgetInstance) == true)
		{
			NotificationTextWidgetInstance->AddToViewport();
			
		}
	}
}

void ABSPlayerController::SetChatMessageString(const FString& InChatMessageString)
{
	ChatMessageString = InChatMessageString;

	if (IsLocalController() == true)
	{
		ABSPlayerState* BSPS = GetPlayerState<ABSPlayerState>();
		if (IsValid(BSPS) == true)
		{
			FString CombinedMessageString = BSPS->PlayerNameString + TEXT(": ") + InChatMessageString;

			ServerRPCPrintChatMessageString(CombinedMessageString);
		}
	}
}

void ABSPlayerController::PrintChatMessageString(const FString& InChatMessageString)
{
	Baseball_ProjectFunctionLibrary::MyPrintString(this, InChatMessageString, 10.f);

}

void ABSPlayerController::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, NotificationText);
}

void ABSPlayerController::ClientRPCPrintChatMessageString_Implementation(const FString& InChatMessageString)
{
	PrintChatMessageString(InChatMessageString);

}

void ABSPlayerController::ServerRPCPrintChatMessageString_Implementation(const FString& InChatMessageString)
{

	AGameModeBase* GM = UGameplayStatics::GetGameMode(this);
	if (IsValid(GM) == true)
	{
		ABSGameModeBase* BSGM = Cast<ABSGameModeBase>(GM);
		if (IsValid(BSGM) == true)
		{
			BSGM->PrintChatMessageString(this, InChatMessageString);
		}
	}
}


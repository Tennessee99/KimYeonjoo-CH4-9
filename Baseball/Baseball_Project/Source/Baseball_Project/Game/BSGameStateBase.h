// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "BSGameStateBase.generated.h"

/**
 * 
 */
UCLASS()
class BASEBALL_PROJECT_API ABSGameStateBase : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCBroadcastLoginMessage(const FString& InNameString = FString(TEXT("XXXXXXX")));


	/*virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;


	UPROPERTY(Replicated, BlueprintReadOnly)
	int32 RemainingTurnTime;

	UPROPERTY(Replicated, BlueprintReadOnly)
	bool bIsTurnActive;*/


};

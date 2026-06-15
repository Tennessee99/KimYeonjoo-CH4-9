// Fill out your copyright notice in the Description page of Project Settings.


#include "BSGameModeBase.h"

#include "BSGameStateBase.h"
#include "Player/BSPlayerController.h"
#include "EngineUtils.h"
#include "Player/BSPlayerState.h"
//#include "TimerManager.h"

void ABSGameModeBase::OnPostLogin(AController* NewPlayer)
{

	Super::OnPostLogin(NewPlayer);

	ABSPlayerController* BSPlayerController = Cast<ABSPlayerController>(NewPlayer);
	if (IsValid(BSPlayerController) == true)
	{
		BSPlayerController->NotificationText = FText::FromString(TEXT("Connected to the game server."));

		AllPlayerControllers.Add(BSPlayerController);

		ABSPlayerState* BSPS = BSPlayerController->GetPlayerState<ABSPlayerState>();
		if (IsValid(BSPS) == true)
		{
			BSPS->PlayerNameString = TEXT("Player") + FString::FromInt(AllPlayerControllers.Num());
		}

		ABSGameStateBase* BSGameStateBase = GetGameState<ABSGameStateBase>();
		if (IsValid(BSGameStateBase) == true)
		{
			BSGameStateBase->MulticastRPCBroadcastLoginMessage(BSPS->PlayerNameString);
		}
	}
}



FString ABSGameModeBase::GenerateSecretNumber()
{
	TArray<int32> Numbers;
	for (int32 i = 1; i <= 9; ++i)
	{
		Numbers.Add(i);
	}

	FMath::RandInit(FDateTime::Now().GetTicks());
	Numbers = Numbers.FilterByPredicate([](int32 Num) { return Num > 0; });

	FString Result;
	for (int32 i = 0; i < 3; ++i)
	{
		int32 Index = FMath::RandRange(0, Numbers.Num() - 1);
		Result.Append(FString::FromInt(Numbers[Index]));
		Numbers.RemoveAt(Index);
	}

	return Result;
}


bool ABSGameModeBase::IsGuessNumberString(const FString& InNumberString)
{
	bool bCanPlay = false;

	do {

		if (InNumberString.Len() != 3)
		{
			break;
		}

		bool bIsUnique = true;
		TSet<TCHAR> UniqueDigits;
		for (TCHAR C : InNumberString)
		{
			if (FChar::IsDigit(C) == false || C == '0')
			{
				bIsUnique = false;
				break;
			}

			UniqueDigits.Add(C);
		}

		if (UniqueDigits.Num() <= 2)
		{
			bIsUnique = false;
		}

		if (bIsUnique == false)
		{
			break;
		}

		bCanPlay = true;

	} while (false);

	return bCanPlay;
}


FString ABSGameModeBase::JudgeResult(const FString& InSecretNumberString, const FString& InGuessNumberString)
{
	int32 StrikeCount = 0, BallCount = 0;

	for (int32 i = 0; i < 3; ++i)
	{
		if (InSecretNumberString[i] == InGuessNumberString[i])
		{
			StrikeCount++;
		}
		else
		{
			FString PlayerGuessChar = FString::Printf(TEXT("%c"), InGuessNumberString[i]);
			if (InSecretNumberString.Contains(PlayerGuessChar))
			{
				BallCount++;
			}
		}
	}

	if (StrikeCount == 0 && BallCount == 0)
	{
		return TEXT("OUT");
	}

	return FString::Printf(TEXT("%dS%dB"), StrikeCount, BallCount);
}

void ABSGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	SecretNumberString = GenerateSecretNumber();

	//StartTurnTimer();
}

void ABSGameModeBase::PrintChatMessageString(ABSPlayerController* InChattingPlayerController, const FString& InChatMessageString)
{
	FString ChatMessageString = InChatMessageString;
	int Index = InChatMessageString.Len() - 3;
	FString GuessNumberString = InChatMessageString.RightChop(Index);

	const bool bIsGuessNumber = IsGuessNumberString(GuessNumberString);
	

	if (bIsGuessNumber == true)
	{
		FString JudgeResultString = JudgeResult(SecretNumberString, GuessNumberString);

		IncreaseGuessCount(InChattingPlayerController);

		for (TActorIterator<ABSPlayerController> It(GetWorld()); It; ++It)
		{
			ABSPlayerController* BSPlayerController = *It;
			if (IsValid(BSPlayerController) == true)
			{
				FString CombinedMessageString = InChatMessageString + TEXT("->") + JudgeResultString;
				BSPlayerController->ClientRPCPrintChatMessageString(CombinedMessageString);

			}
		}
				int32 StrikeCount = FCString::Atoi(*JudgeResultString.Left(1));
				JudgeGame(InChattingPlayerController, StrikeCount);
		
	}
	else
	{
		for (TActorIterator<ABSPlayerController> It(GetWorld()); It; ++It)
		{
			ABSPlayerController* BSPlayerController = *It;
			if (IsValid(BSPlayerController) == true)
			{
				BSPlayerController->ClientRPCPrintChatMessageString(InChatMessageString);
			}
		}
	}

}

void ABSGameModeBase::IncreaseGuessCount(ABSPlayerController* InChattingPlayerController)
{
	ABSPlayerState* BSPS = InChattingPlayerController->GetPlayerState<ABSPlayerState>();
	if (IsValid(BSPS) == true)
	{
		BSPS->CurrentGuessCount++;
	}
}

void ABSGameModeBase::ResetGame()
{
	
	SecretNumberString = GenerateSecretNumber();
	for (const auto& BSPlayerController : AllPlayerControllers)
	{
		if (BSPlayerController.IsValid() == false)
		{
			continue;
		}
		ABSPlayerState* BSPS = BSPlayerController->GetPlayerState<ABSPlayerState>();
		if (IsValid(BSPS) == true)
		{
			BSPS->CurrentGuessCount = 0;
		}
	}

	//StartTurnTimer();
}

void ABSGameModeBase::JudgeGame(ABSPlayerController* InChattingPlayerController, int InStrikeCount)
{
	if (3 == InStrikeCount)
	{
		ABSPlayerState* BSPS = InChattingPlayerController->GetPlayerState<ABSPlayerState>();
		for (const auto& BSPlayerController : AllPlayerControllers)
		{
			if (IsValid(BSPS) == true)
			{
				FString CombinedMessageString = BSPS->PlayerNameString + TEXT(" has won the game");
				BSPlayerController->NotificationText = FText::FromString(CombinedMessageString);

				ResetGame();
			}
		}
	}
	else
	{
		bool bIsDraw = true;
		for (const auto& BSPlayerController : AllPlayerControllers)
		{
			ABSPlayerState* BSPS = BSPlayerController->GetPlayerState<ABSPlayerState>();
			if (IsValid(BSPS) == true)
			{
				if (BSPS->CurrentGuessCount < BSPS->MaxGuessCount)
				{
					bIsDraw = false;
					break;
				}
			}
		}
	
	
		if (true == bIsDraw)
		{
			for (const auto& BSPlayerController : AllPlayerControllers)
			{
				BSPlayerController->NotificationText = FText::FromString(TEXT("Draw.."));

				ResetGame();
			}
		}
	
	}


}

//void ABSGameModeBase::StartTurnTimer()
//{
//
//	ABSGameStateBase* BSGameState = GetGameState<ABSGameStateBase>();
//	if (IsValid(BSGameState) == false)
//	{
//		return;
//	}
//
//	BSGameState->RemainingTurnTime = TurnTimeLimit;
//	BSGameState->bIsTurnActive = true;
//
//	GetWorldTimerManager().ClearTimer(TurnTimerHandle);
//	GetWorldTimerManager().SetTimer(
//		TurnTimerHandle,
//		this,
//		&ThisClass::TickTurnTimer,
//		1.0f,
//		true
//	);
//}
//
//void ABSGameModeBase::TickTurnTimer()
//{
//	ABSGameStateBase* BSGameState = GetGameState<ABSGameStateBase>();
//	if (IsValid(BSGameState) == false)
//	{
//		return;
//	}
//
//	BSGameState->RemainingTurnTime--;
//
//	if (BSGameState->RemainingTurnTime <= 0)
//	{
//		BSGameState->RemainingTurnTime = 0;
//		BSGameState->bIsTurnActive = false;
//
//		GetWorldTimerManager().ClearTimer(TurnTimerHandle);
//
//		for (const auto& BSPlayerController : AllPlayerControllers)
//		{
//			if (BSPlayerController.IsValid() == true)
//			{
//				BSPlayerController->ClientRPCPrintChatMessageString(TEXT("Time Over,,"));
//			}
//		}
//	}
//}
//
//bool ABSGameModeBase::CanPlayTurn() const
//{
//	const ABSGameStateBase* BSGameState = GetGameState<ABSGameStateBase>();
//	if (IsValid(BSGameState) == false)
//	{
//		return false;
//	}
//
//	return BSGameState->bIsTurnActive && BSGameState->RemainingTurnTime > 0;
//
//}




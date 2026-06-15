// Fill out your copyright notice in the Description page of Project Settings.


#include "BSChatInput.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "Player/BSPlayerController.h"
#include "Game/BSGameStateBase.h"
#include "TimerManager.h"

void UBSChatInput::NativeConstruct()
{
	Super::NativeConstruct();

	if (EditableTextBox_ChatInput->OnTextCommitted.IsAlreadyBound(this, &ThisClass::OnChatInputTextCommitted) == false)
	{
		EditableTextBox_ChatInput->OnTextCommitted.AddDynamic(this, &ThisClass::OnChatInputTextCommitted);

	}
}

void UBSChatInput::NativeDestruct()
{
	Super::NativeDestruct();

	if (EditableTextBox_ChatInput->OnTextCommitted.IsAlreadyBound(this, &ThisClass::OnChatInputTextCommitted) == true)
	{
		EditableTextBox_ChatInput->OnTextCommitted.RemoveDynamic(this, &ThisClass::OnChatInputTextCommitted);
	}
}
//
//void UBSChatInput::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
//{
//	Super::NativeTick(MyGeometry, InDeltaTime);
//
//		UpdateTimerText();
//}

void UBSChatInput::OnChatInputTextCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (CommitMethod == ETextCommit::OnEnter)
	{
		APlayerController* OwningPlayerController = GetOwningPlayer();
		if (IsValid(OwningPlayerController) == true)
		{
			ABSPlayerController* OwningBSPlayerController = Cast<ABSPlayerController>(OwningPlayerController);
			if (IsValid(OwningBSPlayerController) == true)
			{
				OwningBSPlayerController->SetChatMessageString(Text.ToString());

				EditableTextBox_ChatInput->SetText(FText());

				//입력후 커서 다시 잡기
				FTimerHandle FocusTimerHandle;
				GetWorld()->GetTimerManager().SetTimer(
					FocusTimerHandle,
					this,
					&ThisClass::FocusChatInput,
					0.01,
					false
				);
			}
		}
	}
}

//void UBSChatInput::UpdateTimerText()
//{
//
//	ABSGameStateBase* BSGameState = GetWorld()->GetGameState<ABSGameStateBase>();
//	if (IsValid(BSGameState) == false)
//	{
//		return;
//	}
//
//	if (IsValid(TextBlock_Timer) == true)
//	{
//		FString TimerString = FString::Printf(TEXT("Time:%d"), BSGameState->RemainingTurnTime);
//		TextBlock_Timer->SetText(FText::FromString(TimerString));
//
//	}
//
//	if (IsValid(EditableTextBox_ChatInput) == true)
//	{
//		const bool bCanInput = BSGameState->bIsTurnActive && BSGameState->RemainingTurnTime > 0;
//		EditableTextBox_ChatInput->SetIsEnabled(bCanInput);
//	}
//}
//
void UBSChatInput::FocusChatInput()
{
	if (IsValid(EditableTextBox_ChatInput) == true)
	{
		EditableTextBox_ChatInput->SetKeyboardFocus();
	}
}

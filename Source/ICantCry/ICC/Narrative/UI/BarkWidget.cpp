// Fill out your copyright notice in the Description page of Project Settings.


#include "BarkWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"


#include "ICantCry/ICC/Narrative/Data/DialogueAsset.h"
#include "ICantCry/ICC/Narrative/GameplayEvent.h"
#include "ICantCry/ICC/Actors/Player/ICC_Player.h"

void UBarkWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (BtnNext) BtnNext->OnClicked.AddDynamic(this, &UBarkWidget::DisplayNextLine);

    // Blocca il player: solo UI + cursore + focus sul widget
    if (APlayerController* PC = GetOwningPlayer())
    {
        PC->bShowMouseCursor = true;
        FInputModeUIOnly Mode;
        Mode.SetWidgetToFocus(TakeWidget());
        PC->SetInputMode(Mode);

        PC->FlushPressedKeys();       // libera i tasti tenuti premuti (es. W) -> niente "latch"
        PC->SetIgnoreMoveInput(true); // blocca il movimento del pawn finché il bark è aperto
    }
}

void UBarkWidget::StartBark(UDialogueAsset* NewBark)
{
    if (!NewBark)
    {
        EndBark();
        return;
    }

    CurrentBark = NewBark;
    CurrentLineIndex = 0;
    ApplyStyle();
    DisplayNextLine();
}

void UBarkWidget::DisplayNextLine()
{
    // Se il typewriter è in corso, completa la riga subito (skip)
    if (GetWorld()->GetTimerManager().IsTimerActive(TypewriterTimerHandle))
    {
        FinishLineInstantly();
        return;
    }

    if (!CurrentBark)
    {
        EndBark();
        return;
    }

    if (CurrentBark->Lines.IsValidIndex(CurrentLineIndex))
    {
        const FDialogueLine& Line = CurrentBark->Lines[CurrentLineIndex];

        // Eventi di riga: passiamo il Player reale (stesso fix del DialogueWidget)
        AICC_Player* Player = Cast<AICC_Player>(GetOwningPlayerPawn());
        for (UGameplayEvent* Event : Line.Events)
        {
            if (Event) Event->ExecuteEvent(Player, this);
        }

        ApplyStyle();

        FullTextCurrentLine = Line.Text.ToString();
        CurrentCharacterIndex = 0;

        // Nome + ritratto (opzionali)
        /*if (CurrentBark->NPCProfile)
        {
            if (TextNPCName) TextNPCName->SetText(CurrentBark->NPCProfile->NPCName);

            if (NPCFaceImage)
            {
                const FGameplayTag TargetTag = Line.EmotionTag;
                if (CurrentBark->NPCProfile->Portraits.Contains(TargetTag))
                {
                    NPCFaceImage->SetBrushFromTexture(CurrentBark->NPCProfile->Portraits[TargetTag].LoadSynchronous());
                }
                else if (CurrentBark->NPCProfile->Portraits.Num() > 0)
                {
                    TArray<FGameplayTag> Keys;
                    CurrentBark->NPCProfile->Portraits.GetKeys(Keys);
                    NPCFaceImage->SetBrushFromTexture(CurrentBark->NPCProfile->Portraits[Keys[0]].LoadSynchronous());
                }
            }
        }*/

        // Testo: typewriter o istantaneo
        if (CurrentBark->bUseTypewriterEffect && TextDialogueContent)
        {
            TextDialogueContent->SetText(FText::GetEmpty());
            const float Speed = FMath::Max(0.01f, CurrentBark->TypewriterSpeed);
            GetWorld()->GetTimerManager().SetTimer(TypewriterTimerHandle, this, &UBarkWidget::OnTypewriterTick, Speed, true);
        }
        else if (TextDialogueContent)
        {
            TextDialogueContent->SetText(Line.Text);
        }

        CurrentLineIndex++;
    }
    else
    {
        // Niente più righe: chiudi e ridai input al player
        EndBark();
    }
}

void UBarkWidget::EndBark()
{
    GetWorld()->GetTimerManager().ClearTimer(TypewriterTimerHandle);

    if (APlayerController* PC = GetOwningPlayer())
    {
        PC->SetIgnoreMoveInput(false); // riabilita il movimento
        PC->bShowMouseCursor = false;
        PC->SetInputMode(FInputModeGameOnly());
    }

    RemoveFromParent();
}

void UBarkWidget::ApplyStyle()
{
    if (!CurrentBark || !TextDialogueContent) return;

    TextDialogueContent->SetColorAndOpacity(FSlateColor(CurrentBark->DialogueColor));
    if (CurrentBark->DialogueFont.HasValidFont())
    {
        TextDialogueContent->SetFont(CurrentBark->DialogueFont);
    }
}

void UBarkWidget::OnTypewriterTick()
{
    if (CurrentCharacterIndex < FullTextCurrentLine.Len())
    {
        CurrentCharacterIndex++;
        if (TextDialogueContent)
        {
            TextDialogueContent->SetText(FText::FromString(FullTextCurrentLine.Left(CurrentCharacterIndex)));
        }
    }
    else
    {
        GetWorld()->GetTimerManager().ClearTimer(TypewriterTimerHandle);
    }
}

void UBarkWidget::FinishLineInstantly()
{
    GetWorld()->GetTimerManager().ClearTimer(TypewriterTimerHandle);
    if (TextDialogueContent)
    {
        TextDialogueContent->SetText(FText::FromString(FullTextCurrentLine));
    }
    // Non incremento l'indice: il prossimo click avanza
}
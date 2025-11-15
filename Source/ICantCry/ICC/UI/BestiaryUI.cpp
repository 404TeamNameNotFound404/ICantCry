// Fill out your copyright notice in the Description page of Project Settings.
#include "BestiaryUI.h"
#include "ICantCry/ICC/Mechanics/Core/Dontdestroyonload/ICantCryGameInstance.h"

void UBestiaryUI::NativeConstruct()
{
    Super::NativeConstruct();

    ICantCryGameInstance = Cast<UICantCryGameInstance>(GetGameInstance());

    CurrentPage = EEmotionPage::Stats;

    
    // // [DEBUG_START] - Emotion unlock debug
    // if (EmotionsData && ICantCryGameInstance)
    // {
    //     for (const TPair<EEmotionType, FEmotionStats> &Pair : EmotionsData->EmotionStats)
    //     {
    //         EEmotionType Emotion = Pair.Key;
    //         if (Emotion != EEmotionType::Default)
    //         {
    //             ICantCryGameInstance->UnlockedEmotions.Add(Emotion);
    //         }
    //     }
    //     UE_LOG(LogTemp, Warning, TEXT("[DEBUG] Unlocked %d emotions"), ICantCryGameInstance->UnlockedEmotions.Num());
    // }
    // // [DEBUG_END]

    
    // // [DEBUG_START] - Note unlock debug
    // if (NoteData && ICantCryGameInstance)
    // {
    //     int32 NotesUnlocked = 0;
    //     for (const auto &Pair : NoteData->Notes)
    //     {
    //         const FString &NoteKey = Pair.Key;
    //         if (!ICantCryGameInstance->CollectedNotes.Contains(NoteKey))
    //         {
    //             ICantCryGameInstance->CollectedNotes.Add(NoteKey);
    //             NotesUnlocked++;
    //         }
    //     }
    //     UE_LOG(LogTemp, Warning, TEXT("[DEBUG] Unlocked %d notes"), NotesUnlocked);
    // }
    // // [DEBUG_END]

    if (EmotionsData)
    {
        SetupEmotionsData(EmotionsData);

        
        for (const TPair<EEmotionType, FEmotionStats> &Pair : EmotionsData->EmotionStats)
        {
            if (Pair.Key != EEmotionType::Default)
            {
                CurrentEmotion = Pair.Key; // Imposta la prima emotion come corrente
                UE_LOG(LogTemp, Warning, TEXT("Auto-selected emotion: %s"), *UEnum::GetValueAsString(CurrentEmotion));
                break;
            }
        }

        RefreshEmotionButtons();
    }

    if (NoteData && ICantCryGameInstance)
    {

        SetupNoteData(NoteData);
        UpdateNotesDisplay();
    }

    InitializeSwitchers();

    UpdatePageContent();

    
}

void UBestiaryUI::InitializeSwitchers()
{
    
    if (ContentSwitcher)
    {
        
        ContentSwitcher->SetActiveWidget(EmotionDetailsPage); 
        UE_LOG(LogTemp, Warning, TEXT("ContentSwitcher initialized to EmotionDetailsPage"));
    }

   
    if (EmotionPageSwitcher)
    {
        EmotionPageSwitcher->SetActiveWidgetIndex(static_cast<int32>(CurrentPage));
        UE_LOG(LogTemp, Warning, TEXT("EmotionPageSwitcher initialized to page: %d"), static_cast<int32>(CurrentPage));
    }
}

void UBestiaryUI::SetupEmotionsData(UEmotionsData *Data)
{
    EmotionsData = Data;

    if (!EmotionsData)
    {
        DebugHelper::LogError("BestiaryUI::SetupEmotionsData - EmotionsData is null");

        // CurrentEmotion = EEmotionType::Default;

        // UpdateEmotionDetails(CurrentEmotion);
        return;
    }

    RefreshEmotionButtons();
    UpdatePageContent();
}

void UBestiaryUI::SetupNoteData(UNoteData *Data)
{
    NoteData = Data;
    UpdateNotesDisplay();
}

void UBestiaryUI::RefreshEmotionButtons()
{

    if (!EmotionsList)
    {
        DebugHelper::LogError("BestiaryUI::RefreshEmotionButtons - EmotionsList is null");
        return;
    }

    if (!EmotionsData)
    {
        DebugHelper::LogError("BestiaryUI::RefreshEmotionButtons - EmotionsData is null");
        return;
    }

    if (!ICantCryGameInstance)
    {
        DebugHelper::LogError("BestiaryUI::RefreshEmotionButtons - GameInstance is null");
        return;
    }

    EmotionsList->ClearChildren();
    EmotionButtonMap.Empty();

    // // DEBUG: Verifica le emotion sbloccate
    // // [DEBUG_START] - Unlocked emotions check
    // UE_LOG(LogTemp, Warning, TEXT("[DEBUG] === UNLOCKED EMOTIONS IN GAMEINSTANCE ==="));
    // UE_LOG(LogTemp, Warning, TEXT("[DEBUG] Unlocked emotions count: %d"), ICantCryGameInstance->UnlockedEmotions.Num());
    // for (EEmotionType UnlockedEmotion : ICantCryGameInstance->UnlockedEmotions)
    // {
    //     UE_LOG(LogTemp, Warning, TEXT("[DEBUG] Unlocked: %s"), *UEnum::GetValueAsString(UnlockedEmotion));
    // }
    // // [DEBUG_END]

    for (const TPair<EEmotionType, FEmotionStats> &Pair : EmotionsData->EmotionStats)
    {
        const EEmotionType Emotion = Pair.Key;
        if (Emotion == EEmotionType::Default)
            continue;

        //Verifica corretta se l'emotion è sbloccata
        bool bIsUnlocked = ICantCryGameInstance->UnlockedEmotions.Contains(Emotion);

        // [DEBUG_START] - Emotion unlock status
        UE_LOG(LogTemp, Warning, TEXT("[DEBUG] Emotion %s - Unlocked in GameInstance: %s"),
               *UEnum::GetValueAsString(Emotion),
               bIsUnlocked ? TEXT("YES") : TEXT("NO"));
        // [DEBUG_END]

        if (EmotionButton && EmotionsList)
        {
            UEmotionButtonWidget *Btn = CreateWidget<UEmotionButtonWidget>(this, EmotionButton);
            if (Btn)
            {
                Btn->SetupButton(Emotion);
                Btn->SetLocked(!bIsUnlocked);  // Se è sbloccata, SetLocked(false) //!bIsUnlocked

                Btn->SetSelected(Emotion == CurrentEmotion);
                Btn->OnEmotionSelected.AddDynamic(this, &UBestiaryUI::OnEmotionSelected);

                EmotionsList->AddChild(Btn);
                EmotionButtonMap.Add(Emotion, Btn);
            }
        }
    }
}

bool UBestiaryUI::IsOpen() const
{
    return bIsOpen;
}

void UBestiaryUI::SetIsOpen(const bool &Value)
{
    bIsOpen = Value;
}

void UBestiaryUI::UpdateEmotionDetails(EEmotionType Emotion)
{
    bool bIsUnlocked = ICantCryGameInstance && ICantCryGameInstance->UnlockedEmotions.Contains(Emotion);

    if (!bIsUnlocked || Emotion == EEmotionType::Default)
    {
        // Emotion bloccata - mostra tutto "???"
        if (TitleText)
        {
            TitleText->SetText(FText::FromString(TEXT("???")));
            TitleText->SetVisibility(ESlateVisibility::Visible);
        }
        if (WeaknessesText)
        {
            WeaknessesText->SetText(FText::FromString(TEXT("???")));
            WeaknessesText->SetVisibility(ESlateVisibility::Visible);
        }
        if (ResistancesText)
        {
            ResistancesText->SetText(FText::FromString(TEXT("???")));
            ResistancesText->SetVisibility(ESlateVisibility::Visible);
        }
        if (MovesText)
        {
            MovesText->SetText(FText::FromString(TEXT("???")));
            MovesText->SetVisibility(ESlateVisibility::Visible);
        }
        if (NarrativeInfoText)
        {
            NarrativeInfoText->SetText(FText::FromString(TEXT("???")));
            NarrativeInfoText->SetVisibility(ESlateVisibility::Collapsed);
        }
        if (BattleBarksText)
        {
            BattleBarksText->SetText(FText::FromString(TEXT("???")));
            BattleBarksText->SetVisibility(ESlateVisibility::Collapsed);
        }

        if (ContentImage)
        {
            ContentImage->SetVisibility(ESlateVisibility::Hidden);
        }

        // Aggiorna indicatore pagina
        if (PageIndicatorText)
        {
            const int32 PageIndex = static_cast<int32>(CurrentPage) + 1;
            const int32 PageCount = static_cast<int32>(EEmotionPage::Count);
            PageIndicatorText->SetText(FText::FromString(FString::Printf(TEXT("%d / %d"), PageIndex, PageCount)));
        }

        return;
    }

    const FEmotionStats *Stats = EmotionsData->EmotionStats.Find(Emotion);
    if (!Stats)
    {
        return;
    }

    // Aggiorna immagine emotion
    if (ContentImage)
    {
        if (Stats->EmotionTexture)
        {
            ContentImage->SetBrushFromTexture(Stats->EmotionTexture);
            ContentImage->SetVisibility(ESlateVisibility::Visible);
        }
        else
        {
            ContentImage->SetVisibility(ESlateVisibility::Hidden);
        }
    }

    // Aggiorna titolo emotion
    if (TitleText)
    {
        if (const UEnum *EnumPtr = StaticEnum<EEmotionType>())
        {
            FText EmotionName = EnumPtr->GetDisplayNameTextByValue((int64)Emotion);
            TitleText->SetText(EmotionName);
            TitleText->SetVisibility(ESlateVisibility::Visible);
        }
    }

    // Determina la pagina corrente
    bool bIsStatsPage = (CurrentPage == EEmotionPage::Stats);
    bool bIsNarrativePage = (CurrentPage == EEmotionPage::Narrative);

    // Pagina STATS - Weaknesses, Resistances, Moves
    if (WeaknessesText)
    {
        if (bIsStatsPage)
        {
            TArray<FString> WeakNames;
            for (EEmotionType W : Stats->Weaknesses)
            {
                if (const UEnum *EnumPtr = StaticEnum<EEmotionType>())
                {
                    WeakNames.Add(EnumPtr->GetDisplayNameTextByValue((int64)W).ToString());
                }
            }
            FString WeaknessesStr = WeakNames.Num() > 0 ? FString::Join(WeakNames, TEXT(", ")) : "None";
            WeaknessesText->SetText(FText::FromString("Weaknesses: " + WeaknessesStr));
            WeaknessesText->SetVisibility(ESlateVisibility::Visible);
        }
        else
        {
            WeaknessesText->SetVisibility(ESlateVisibility::Collapsed);
        }
    }

    if (ResistancesText)
    {
        if (bIsStatsPage)
        {
            TArray<FString> ResistancesName;
            for (EEmotionType R : Stats->Resistances)
            {
                if (const UEnum *EnumPtr = StaticEnum<EEmotionType>())
                {
                    ResistancesName.Add(EnumPtr->GetDisplayNameTextByValue((int64)R).ToString());
                }
            }
            FString ResistancesStr = ResistancesName.Num() > 0 ? FString::Join(ResistancesName, TEXT(", ")) : "None";
            ResistancesText->SetText(FText::FromString("Resistances: " + ResistancesStr));
            ResistancesText->SetVisibility(ESlateVisibility::Visible);
        }
        else
        {
            ResistancesText->SetVisibility(ESlateVisibility::Collapsed);
        }
    }

    if (MovesText)
    {
        if (bIsStatsPage)
        {
            FString MovesStr = Stats->Moves.Num() > 0 ? FString::Join(Stats->Moves, TEXT(", ")) : "None";
            MovesText->SetText(FText::FromString("Moves: " + MovesStr));
            MovesText->SetVisibility(ESlateVisibility::Visible);
        }
        else
        {
            MovesText->SetVisibility(ESlateVisibility::Collapsed);
        }
    }

    // Pagina NARRATIVE - NarrativeInfo e BattleBarks
    if (NarrativeInfoText)
    {
        if (bIsNarrativePage)
        {
            NarrativeInfoText->SetText(FText::FromString(Stats->NarrativeInformation));
            NarrativeInfoText->SetVisibility(ESlateVisibility::Visible);
        }
        else
        {
            NarrativeInfoText->SetVisibility(ESlateVisibility::Collapsed);
        }
    }

    if (BattleBarksText)
    {
        if (bIsNarrativePage)
        {
            FString JoinedBarks;
            for (int32 i = 0; i < Stats->BattleBarks.Num(); ++i)
            {
                JoinedBarks += Stats->BattleBarks[i];
                if (i < Stats->BattleBarks.Num() - 1)
                {
                    JoinedBarks += TEXT("\n");
                }
            }
            BattleBarksText->SetText(FText::FromString(JoinedBarks));
            BattleBarksText->SetVisibility(ESlateVisibility::Visible);
        }
        else
        {
            BattleBarksText->SetVisibility(ESlateVisibility::Collapsed);
        }
    }

    // Aggiorna EmotionPageSwitcher
    if (EmotionPageSwitcher)
    {
        EmotionPageSwitcher->SetActiveWidgetIndex(static_cast<int32>(CurrentPage));
    }

    // Aggiorna indicatore pagina
    if (PageIndicatorText)
    {
        const int32 PageIndex = static_cast<int32>(CurrentPage) + 1;
        const int32 PageCount = static_cast<int32>(EEmotionPage::Count);
        PageIndicatorText->SetText(FText::FromString(FString::Printf(TEXT("%d / %d"), PageIndex, PageCount)));
    }
}

void UBestiaryUI::UpdatePageContent()
{
    // calls UpdateEmotionDetails which takes care of everything
    // based on the current emotion and current page
    UpdateEmotionDetails(CurrentEmotion);
}

void UBestiaryUI::UpdateNotesDisplay()
{
    if (!NotesList)
    {
        UE_LOG(LogTemp, Error, TEXT("UpdateNotesDisplay: NotesList is null!"));
        return;
    }

    NotesList->ClearChildren();

    if (!NoteData)
    {
        UE_LOG(LogTemp, Error, TEXT("UpdateNotesDisplay: NoteData is null"));
        return;
    }

    if (!ICantCryGameInstance)
    {
        UE_LOG(LogTemp, Error, TEXT("UpdateNotesDisplay: GameInstance is null"));
        return;
    }

    TArray<FString> NoteKeys;
    NoteData->Notes.GetKeys(NoteKeys);

    // DEBUG: Info note
    // [DEBUG_START] - Note display info
    UE_LOG(LogTemp, Warning, TEXT("[DEBUG] === NOTES DISPLAY ==="));
    UE_LOG(LogTemp, Warning, TEXT("[DEBUG] Total notes: %d, Collected: %d"),
           NoteKeys.Num(), ICantCryGameInstance->CollectedNotes.Num());
    // [DEBUG_END]

    for (int32 i = 0; i < NoteKeys.Num(); i++)
    {
        const FString &NoteKey = NoteKeys[i];
        const FNoteContent &NoteContent = NoteData->Notes[NoteKey];
        bool bIsCollected = ICantCryGameInstance->CollectedNotes.Contains(NoteKey);

        // DEBUG: Dettaglio singola nota
        // [DEBUG_START] - Single note detail
        UE_LOG(LogTemp, Warning, TEXT("[DEBUG] Note %d: %s -> '%s' (Collected: %s)"),
               i, *NoteKey, *NoteContent.Title, bIsCollected ? TEXT("YES") : TEXT("NO"));
        // [DEBUG_END]

        if (NoteButton)
        {
            UNoteButtonWidget *NoteButtonEntry = CreateWidget<UNoteButtonWidget>(this, NoteButton);
            if (NoteButtonEntry)
            {
                // Se sbloccata, mostra il titolo reale
                FString DisplayName = bIsCollected ? NoteContent.Title : TEXT("???");
                NoteButtonEntry->SetupNote(DisplayName);
                NoteButtonEntry->SetNoteKey(NoteKey);
                NoteButtonEntry->OnNoteSelected.AddDynamic(this, &UBestiaryUI::OnNoteSelected);
                NotesList->AddChild(NoteButtonEntry);

                // DEBUG: Bottone creato
                // [DEBUG_START] - Button creation
                UE_LOG(LogTemp, Warning, TEXT("[DEBUG] Created button: %s"), *DisplayName);
                // [DEBUG_END]
            }
        }
    }
}

void UBestiaryUI::OnNoteSelected(FString SelectedNote)
{

    // [DEBUG_START] - Note selection
    UE_LOG(LogTemp, Warning, TEXT("[DEBUG] === NOTE SELECTED ==="));
    UE_LOG(LogTemp, Warning, TEXT("[DEBUG] Received display name: '%s'"), *SelectedNote);
    // [DEBUG_END]

    if (!NoteData || !ICantCryGameInstance)
        return;

    for (auto &Pair : EmotionButtonMap)
    {
        if (Pair.Value)
        {
            Pair.Value->SetSelected(false);
        }
    }

    FString FoundNoteKey = "";
    bool bIsCollected = false;

    // Cerca il bottone cliccato nella lista
    if (NotesList)
    {
        for (int32 i = 0; i < NotesList->GetChildrenCount(); i++)
        {
            UWidget *ChildWidget = NotesList->GetChildAt(i);
            UNoteButtonWidget *NoteBtn = Cast<UNoteButtonWidget>(ChildWidget);

            if (NoteBtn && NoteBtn->GetNoteName() == SelectedNote)
            {
                FoundNoteKey = NoteBtn->GetNoteKey();
                bIsCollected = ICantCryGameInstance->CollectedNotes.Contains(FoundNoteKey);
                break;
            }
        }
    }

    if (!FoundNoteKey.IsEmpty() && NoteData->Notes.Contains(FoundNoteKey))
    {
        const FNoteContent &NoteContent = NoteData->Notes[FoundNoteKey];

        if (!bIsCollected)
        {
            // Nota bloccata - mostra "???"
            if (TitleText)
                TitleText->SetText(FText::FromString(TEXT("???")));
            if (NoteContentText)
                NoteContentText->SetText(FText::FromString(TEXT("???")));
            if (ContentImage)
                ContentImage->SetVisibility(ESlateVisibility::Hidden);
        }
        else
        {
            // Nota sbloccata - mostra contenuto reale
            if (TitleText)
                TitleText->SetText(FText::FromString(NoteContent.Title));
            if (NoteContentText)
                NoteContentText->SetText(FText::FromString(NoteContent.Content));
            if (ContentImage)
            {
                if (NoteContent.NoteImage)
                {
                    ContentImage->SetBrushFromTexture(NoteContent.NoteImage);
                    ContentImage->SetVisibility(ESlateVisibility::Visible);
                }
                else
                {
                    ContentImage->SetVisibility(ESlateVisibility::Hidden);
                }
            }
        }
    }
    else
    {
        // Fallback se la nota non viene trovata
        if (TitleText)
            TitleText->SetText(FText::FromString(TEXT("???")));
        if (NoteContentText)
            NoteContentText->SetText(FText::FromString(TEXT("???")));
        if (ContentImage)
            ContentImage->SetVisibility(ESlateVisibility::Hidden);
    }

    if (NoteContentText)
    {
        NoteContentText->SetVisibility(ESlateVisibility::Visible);
    }

    // Cambia alla pagina delle note
    if (ContentSwitcher)
    {
        ContentSwitcher->SetActiveWidget(NoteDetailsPage);
    }
}

void UBestiaryUI::AddCollectedNote(const FString &NoteKey)
{
    if (ICantCryGameInstance)
    {
        if (!ICantCryGameInstance->CollectedNotes.Contains(NoteKey))
        {
            ICantCryGameInstance->CollectedNotes.Add(NoteKey);
            UpdateNotesDisplay();
        }
    }
}

bool UBestiaryUI::HasNote(const FString &NoteKey) const
{
    if (ICantCryGameInstance)
    {

        return ICantCryGameInstance->CollectedNotes.Contains(NoteKey);
    }
    return false;
}

void UBestiaryUI::OnEmotionSelected(EEmotionType SelectedEmotion)
{

    // [DEBUG_START] - Emotion selected
    UE_LOG(LogTemp, Warning, TEXT("[DEBUG] === ON EMOTION SELECTED ==="));
    UE_LOG(LogTemp, Warning, TEXT("[DEBUG] Selected Emotion: %s"), *UEnum::GetValueAsString(SelectedEmotion));
    UE_LOG(LogTemp, Warning, TEXT("[DEBUG] Current Emotion before: %s"), *UEnum::GetValueAsString(CurrentEmotion));
    // [DEBUG_END]

    CurrentEmotion = SelectedEmotion;

    // Aggiorna selezione bottoni
    for (auto &Pair : EmotionButtonMap)
    {
        if (Pair.Value)
        {
            // Pair.Value->SetSelected(Pair.Key == CurrentEmotion);
            bool bShouldBeSelected = (Pair.Key == CurrentEmotion); // aggiunto
            Pair.Value->SetSelected(bShouldBeSelected);

            // [DEBUG_START] - Button selection
            UE_LOG(LogTemp, Warning, TEXT("[DEBUG] Button %s - Selected: %s"),
                   *UEnum::GetValueAsString(Pair.Key),
                   bShouldBeSelected ? TEXT("YES") : TEXT("NO"));
            // [DEBUG_END]
        }
    }

    
    if (TitleText)
    {
        TitleText->SetVisibility(ESlateVisibility::Visible);
    }

    if (ContentImage)
    {
        ContentImage->SetVisibility(ESlateVisibility::Visible);
    }

    if (NoteContentText)
    {
        // Nascondi il contenuto delle note quando mostri le emotion
        NoteContentText->SetVisibility(ESlateVisibility::Collapsed);
    }

    // Vai alla pagina emotion details
    if (ContentSwitcher)
    {
        ContentSwitcher->SetActiveWidget(EmotionDetailsPage);
        UE_LOG(LogTemp, Warning, TEXT("[DEBUG] Switched to EmotionDetailsPage"));
    }

    // Reimposta alla pagina Stats quando cambi emotion
    CurrentPage = EEmotionPage::Stats;
    if (EmotionPageSwitcher)
    {
        EmotionPageSwitcher->SetActiveWidgetIndex(static_cast<int32>(CurrentPage));
        UE_LOG(LogTemp, Warning, TEXT("[DEBUG] EmotionPageSwitcher reset to Stats page"));
    }

    UpdatePageContent();
}

void UBestiaryUI::ChangePage(int32 Direction)
{
    
    if (ContentSwitcher && ContentSwitcher->GetActiveWidget() == NoteDetailsPage)
    {
        UE_LOG(LogTemp, Warning, TEXT("ChangePage: IGNORED - Currently in NoteDetailsPage"));
        return;
    }

    int32 CurrentIndex = static_cast<int32>(CurrentPage);
    int32 NewPageIndex = CurrentIndex + Direction;
    int32 PageCount = static_cast<int32>(EEmotionPage::Count);

    // Blocca ai limiti
    if (NewPageIndex < 0 || NewPageIndex >= PageCount)
    {
        UE_LOG(LogTemp, Warning, TEXT("ChangePage: BLOCKED at page limit"));
        return;
    }

    CurrentPage = static_cast<EEmotionPage>(NewPageIndex);
    UpdatePageContent();
}

void UBestiaryUI::NextPage()
{
    
    if (ContentSwitcher && ContentSwitcher->GetActiveWidget() == NoteDetailsPage)
    {
        UE_LOG(LogTemp, Warning, TEXT("NextPage: IGNORED - Currently in NoteDetailsPage"));
        return; // Non fare nulla se siamo nelle note
    }

    if (CurrentEmotion != EEmotionType::Default)
    {
        ChangePage(1);
        UE_LOG(LogTemp, Warning, TEXT("NextPage called - New page: %d"), static_cast<int32>(CurrentPage));
    }
    
}

void UBestiaryUI::PreviousPage()
{

    if (ContentSwitcher && ContentSwitcher->GetActiveWidget() == NoteDetailsPage)
    {
        UE_LOG(LogTemp, Warning, TEXT("PreviousPage: IGNORED - Currently in NoteDetailsPage"));
        return; // Non fare nulla se siamo nelle note
    }

    if (CurrentEmotion != EEmotionType::Default)
    {
        ChangePage(-1);
        UE_LOG(LogTemp, Warning, TEXT("PreviousPage called - New page: %d"), static_cast<int32>(CurrentPage));
    }
}

void UBestiaryUI::UnlockEmotion(EEmotionType Emotion)
{
    if (ICantCryGameInstance)
    {
        ICantCryGameInstance->UnlockedEmotions.Add(Emotion);
        RefreshEmotionButtons();
        UpdateEmotionDetails(Emotion);
    }
}


void UBestiaryUI::RefreshNotesFromGameInstance()
{
    if (ICantCryGameInstance)
    {
        UpdateNotesDisplay();
        UE_LOG(LogTemp, Warning, TEXT("[BESTIARY] Notes display refreshed from GameInstance"));
    }
}
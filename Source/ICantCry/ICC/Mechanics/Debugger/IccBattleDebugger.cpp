// Fill out your copyright notice in the Description page of Project Settings.


#include "IccBattleDebugger.h"

#include <ThirdParty/ImGuiLibrary/Private/imgui_internal.h>
#include "imgui.h"
#include "ICantCry/ICC/Actors/Player/ICC_Player.h"
#include "ICantCry/ICC/Mechanics/Core/Dontdestroyonload/ICantCryGameInstance.h"

// Sets default values
AIccBattleDebugger::AIccBattleDebugger()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

static ImVec4 Grey = ImVec4{0.73f, 0.83f, 0.85f, 1.0f};
static ImVec4 Red = ImVec4{1.0f, 0.0f, 0.0f, 1.0f};
static ImVec4 Green = ImVec4{0.0f, 1.0f, 0.0f, 1.0f};
static ImVec4 Blue = ImVec4{0.0f, 0.0f, 1.0f, 1.0f};
static ImVec4 Yellow = ImVec4{1.0f, 1.0f, 0.0f, 1.0f};

void AIccBattleDebugger::FillInitialInfo()
{
	const UICantCryGameInstance* Instance = Cast<UICantCryGameInstance>(GetGameInstance());
	Queue = Instance->GetCurrentPlayer()->GetBattleHUD()
	                ->GetBattleHandler()->GetTurnBasedSystem()->GetTurn().Queue;

	if (Queue.IsEmpty()) return;

	for (AICC_Actor* A : Queue)
	{
		if (!A) continue;

		FIccBattleDebuggerData Data;
		Data.TargetName = A->GetName();
		Data.bIsAfflicted = A->GetStatusTracker()->IsAfflicted();
		Data.bIsBuffed = A->GetStatusTracker()->IsBuffed();
		Data.bIsDebuffed = A->GetStatusTracker()->IsDebuffed();
		Data.DebuffCounter = A->GetStatusTracker()->GetDebuffCounter();
		Data.BuffCounter = A->GetStatusTracker()->GetBuffCounter();

		if (AICC_Player* P = Cast<AICC_Player>(A))
		{
			Data.Health = P->GetRuntimeStats().CurrentHealth;
		}
		else if (AMob* E = Cast<AMob>(A))
		{
			Data.Health = E->GetStats().Health;
		}

		if (!A->IsA(AICC_Player::StaticClass()))
		{
			FIccDebuggerDecisionData Decision;
			Decision.TargetName = A->GetName();
			Decision.DecisionPicked = "None";
			Decision.DecisionTable = "None";

			Decisions.Add(Decision);
		}

		FIccDebuggerTrackingData Tracking;
		Tracking.TargetName = A->GetName();
		Tracking.AttackPower = 0.f;
		Tracking.DefencePower = 0.f;
		Tracking.CurrentBuff = "None";
		Tracking.CurrentDebuff = "None";
		Tracking.DamageDealt = 0.f;

		Infos.Add(Data);
		Trackings.Add(Tracking);
	}
}

// Called when the game starts or when spawned
void AIccBattleDebugger::BeginPlay()
{
	Super::BeginPlay();

	GameInstance = Cast<UICantCryGameInstance>(GetGameInstance());
	bDisplay = false;
}

// Called every frame
void AIccBattleDebugger::Tick(float DeltaTime)
{
	if (!bDisplay) return;

	Super::Tick(DeltaTime);

	ImGui::Begin("ICC Battle Debugger");

	ImGui::Text("Debugger");

	DisplayDebuggerData();

	ImGui::End();
}

void AIccBattleDebugger::DisplayDebuggerData()
{
	if (Infos.Num() != Queue.Num())
	{
		Infos.SetNum(Queue.Num()); // match size
		Trackings.SetNum(Queue.Num());
	}

	for (int32 i = 0; i < Queue.Num(); ++i)
	{
		Infos[i].RefreshInfo(Queue[i]);
		Trackings[i].RefreshInfo(Queue[i]);
	}

	for (FIccDebuggerDecisionData& DecisionData : Decisions)
	{
		for (AICC_Actor* A : Queue)
		{
			if (A && A->GetName() == DecisionData.TargetName)
			{
				DecisionData.RefreshInfo(A);
				break;
			}
		}
	}

	if (ImGui::BeginTabBar("DebuggerTabBar"))
	{
		DisplayStats();
		DisplayDecisionTables();
		DisplayTrackers();

		ImGui::EndTabBar();
	}
}

void AIccBattleDebugger::DisplayStats()
{
	if (ImGui::BeginTabItem("General Stats"))
	{
		if (Infos.IsEmpty())
		{
			ImGui::TextColored(Red,"%s","No debug data available");
			ImGui::EndTabItem();
			return;
		}
		
		constexpr ImGuiTableFlags TableFlags = ImGuiTableFlags_BordersInnerV
			| ImGuiTableFlags_RowBg
			| ImGuiTableFlags_SizingFixedFit;

		if (ImGui::BeginTable("BattleDebuggerTable", Infos.Num(), TableFlags))
		{
			ImGui::TableNextRow();
			for (const FIccBattleDebuggerData& Data : Infos)
			{
				ImGui::TableNextColumn();
				ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.0f, 1.0f),
				                   "%s", TCHAR_TO_UTF8(*Data.TargetName));
				ImGui::Separator();
			}

			ImGui::TableNextRow();
			for (FIccBattleDebuggerData& Data : Infos)
			{
				ImGui::TableNextColumn();

				ImGui::Text("Health: %.1f", Data.Health);
				ImGui::Separator();
				ImGui::Text("IsBuffed?");
				ImGui::SameLine();
				ImGui::TextColored(Data.bIsBuffed ? Red : Grey, "%s", Data.bIsBuffed ? "true" : "false");
				ImGui::Separator();
				ImGui::Text("IsDebuffed?");
				ImGui::SameLine();
				ImGui::TextColored(Data.bIsDebuffed ? Red : Grey, "%s", Data.bIsDebuffed ? "true" : "false");
				ImGui::Separator();
				ImGui::Text("IsAfflicted?");
				ImGui::SameLine();
				ImGui::TextColored(Data.bIsAfflicted ? Green : Grey, "%s", Data.bIsAfflicted ? "true" : "false");
				ImGui::Separator();
				ImGui::Text("Buff Counter: %d", Data.BuffCounter);
				ImGui::Separator();
				ImGui::Text("Debuff Counter: %d", Data.DebuffCounter);
				ImGui::Separator();
				ImGui::Text("Status Counter: %d", Data.StatusCounter);
				ImGui::Separator();

				if (Data.bIsBuffed)
				{
					ImGui::Text("Current Buff: ");
					ImGui::SameLine();
					ImGui::TextColored(Blue, "%s", TCHAR_TO_UTF8(*Data.CurrentBuff));
					ImGui::Separator();
				}

				if (Data.bIsDebuffed)
				{
					ImGui::Text("Current DeBuff: ");
					ImGui::SameLine();
					ImGui::TextColored(Blue, "%s", TCHAR_TO_UTF8(*Data.CurrentDebuff));
					ImGui::Separator();
				}

				if (Data.bIsAfflicted)
				{
					ImGui::Text("Current Malus: ");
					ImGui::SameLine();
					ImGui::TextColored(Blue, "%s", TCHAR_TO_UTF8(*Data.CurrentMalus));
					ImGui::Separator();
				}
			}

			ImGui::EndTable();
		}

		ImGui::EndTabItem();
	}
}

void AIccBattleDebugger::DisplayDecisionTables()
{
	if (ImGui::BeginTabItem("Decision Debugger"))
	{
		ImGui::Text("Decision Tables");

		if (Decisions.IsEmpty())
		{
			ImGui::Text("No AI Decision data available.");
			ImGui::EndTabItem();
			return;
		}

		constexpr ImGuiTableFlags TableFlags = ImGuiTableFlags_BordersInnerV
			| ImGuiTableFlags_RowBg
			| ImGuiTableFlags_SizingFixedFit;


		if (ImGui::BeginTable("BattleDebuggerDecisionTable", Decisions.Num(), TableFlags))
		{
			ImGui::TableNextRow();

			for (FIccDebuggerDecisionData& Data : Decisions)
			{
				ImGui::TableNextColumn();
				ImGui::TextColored(Red,"%s", TCHAR_TO_UTF8(*Data.TargetName));
				ImGui::Separator();
			}

			ImGui::TableNextRow();

			for (FIccDebuggerDecisionData& Data : Decisions)
			{
				ImGui::TableNextColumn();
				ImGui::Text("Decision Picked:");
				ImGui::SameLine();
				ImGui::TextColored(Yellow, "%s", TCHAR_TO_UTF8(*Data.DecisionPicked));
				ImGui::Separator();
				ImGui::Text("Decision Table: ");
				ImGui::SameLine();
				ImGui::TextColored(Grey, "%s", TCHAR_TO_UTF8(*Data.DecisionTable));
			}

			ImGui::EndTable();
		}

		ImGui::EndTabItem();
	}
}

void AIccBattleDebugger::DisplayTrackers()
{
	if (ImGui::BeginTabItem("Status Trackers"))
	{
		ImGui::Text("Status Debugger");
		ImGui::Separator();

		if (Trackings.IsEmpty())
		{
			ImGui::TextColored(Red, "%s", "Debug Tracking data empty!");
			ImGui::EndTabItem();
			return;
		}

		constexpr ImGuiTableFlags TableFlags = ImGuiTableFlags_BordersInnerV
			| ImGuiTableFlags_RowBg
			| ImGuiTableFlags_SizingFixedFit;


		if (ImGui::BeginTable("BattleDebuggerTrackings", Trackings.Num(), TableFlags))
		{
			ImGui::TableNextRow();

			for (FIccDebuggerTrackingData& Data : Trackings)
			{
				ImGui::TableNextColumn();
				ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.0f, 1.0f),
				                   "%s", TCHAR_TO_UTF8(*Data.TargetName));
				ImGui::Separator();
			}

			ImGui::TableNextRow();

			for (FIccDebuggerTrackingData& Data : Trackings)
			{
				ImGui::TableNextColumn();
				ImGui::Text("Current Buff: ");
				ImGui::SameLine();
				ImGui::TextColored(Red, "%s", Data.CurrentBuff.IsEmpty() ? "None" : TCHAR_TO_UTF8(*Data.CurrentBuff));
				ImGui::Separator();
				ImGui::Text("Current Debuff: ");
				ImGui::SameLine();
				ImGui::TextColored(Blue, "%s", Data.CurrentDebuff.IsEmpty()
					                               ? "None"
					                               : TCHAR_TO_UTF8(*Data.CurrentDebuff));
				ImGui::Separator();
				ImGui::Text("Attack Power: ");
				ImGui::SameLine();
				ImGui::TextColored(Grey, "%.1f", Data.AttackPower);
				ImGui::Separator();
				ImGui::Text("Defence Power: ");
				ImGui::SameLine();
				ImGui::TextColored(Grey, "%.1f", Data.DefencePower);
				ImGui::Separator();
				ImGui::Text("Damage Dealt: ");
				ImGui::SameLine();
				ImGui::TextColored(Yellow, "%.1f", Data.DamageDealt);
			}

			ImGui::EndTable();
		}

		ImGui::EndTabItem();
	}
}

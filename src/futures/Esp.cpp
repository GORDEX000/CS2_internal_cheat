#include "Esp.h"

uintptr_t Esp::client = (uintptr_t)GetModuleHandle(L"client.dll");

void Esp::drawESP()
{

    if (enabled) {

        float(*ViewMatrix)[4][4] = (float(*)[4][4])(client + Offsets::ClientDll::dwViewMatrix);

        auto localPawn = *(uintptr_t*)(client + Offsets::ClientDll::dwLocalPlayerPawn);
        if (!localPawn)
            return;

        auto entityList = *(uintptr_t*)(client + Offsets::ClientDll::dwEntityList);

        for (int i = 1; i < 64; i++) {
            uintptr_t list_entry1 = *(uintptr_t*)(entityList + (8 * (i & 0x7FFF) >> 9) + 16);
            if (!list_entry1)
                continue;

            uintptr_t playerController = *(uintptr_t*)(list_entry1 + 120 * (i & 0x1FF));
            if (!playerController)
                continue;

            uint32_t playerPawn = *(uint32_t*)(playerController + Offsets::CCSPlayerController::m_hPlayerPawn);
            if (!playerPawn)
                continue;

            uintptr_t list_entry2 = *(uintptr_t*)(entityList + 0x8 * ((playerPawn & 0x7FFF) >> 9) + 16);
            if (!list_entry2)
                continue;

            uintptr_t pCSPlayerPawnPtr = *(uintptr_t*)(list_entry2 + 120 * (playerPawn & 0x1FF));
            if (!pCSPlayerPawnPtr)
                continue;

            if (pCSPlayerPawnPtr == localPawn)
                continue;

            auto localTeam = *(int*)(localPawn + Offsets::C_BaseEntity::m_iTeamNum);

            // skip teammates
            int playerTeam = *(int*)(pCSPlayerPawnPtr + Offsets::C_BaseEntity::m_iTeamNum);
            if (playerTeam == localTeam)
                continue;

            // Name
            char* namePtr = *(char**)(playerController + Offsets::CCSPlayerController::m_sSanitizedPlayerName);
            std::string entityName = namePtr ? std::string(namePtr) : "Unknown";

            // health
            int health = *(int*)(pCSPlayerPawnPtr + Offsets::C_BaseEntity::m_iHealth);
            if (!health || health > 100)
                continue;

            // armor
            int armor = *(int*)(pCSPlayerPawnPtr + Offsets::C_CSPlayerPawn::m_ArmorValue);

            // pos
            Vec3 feetpos = *(Vec3*)(pCSPlayerPawnPtr + Offsets::C_BasePlayerPawn::m_vOldOrigin);
            Vec3 headpos = { feetpos.x + 0.0f, feetpos.y + 0.0f, feetpos.z + 65.0f };

            Vec2 feet, head;
            if (showDebugMenu) {
                ImGui::Text("%s HP: %d Armor: %d X = %f Y = %f Z = %f", entityName.c_str(), health, armor, feetpos.x, feetpos.y, feetpos.z);
            }

            if (feetpos.WorldToScreen(feet, ViewMatrix) && headpos.WorldToScreen(head, ViewMatrix)) {
                float height = fabsf(feet.y - head.y);
                float width = height / 2.0f;
                ImGui::GetForegroundDrawList()->AddRect(ImVec2(feet.x - width / 2, head.y), ImVec2(feet.x + width / 2, feet.y), ImColor(255, 255, 255));

                if (showName) {
                    ImGui::GetForegroundDrawList()->AddText(ImVec2(feet.x - width / 2, head.y - 12), ImColor(245, 223, 222), entityName.c_str());
                }

                if (showHealth) {
                    // Draw gradient health bar (fixed height, independent of current health)
                    float barWidth = 4.0f;
                    float totalBarHeight = fabsf(feet.y - head.y);
                    float barX = feet.x - width / 2 - barWidth - 2; // left of box
                    float barBottomY = feet.y;
                    float barTopY = feet.y - totalBarHeight;

                    int steps = (int)totalBarHeight;
                    for (int i = 0; i < steps; i++) {
                        float t = (float)i / steps; // 0.0 (bottom) → 1.0 (top)
                        // Interpolate color: green at top, red at bottom
                        ImU32 col = ImColor(
                            (int)(255 * (1.0f - t)), // red increases toward bottom
                            (int)(255 * t),          // green decreases toward bottom
                            0
                        );
                        ImGui::GetForegroundDrawList()->AddRectFilled(
                            ImVec2(barX, barBottomY - i - 1),
                            ImVec2(barX + barWidth, barBottomY - i),
                            col
                        );
                    }

                    // Draw grey overlay for missing health
                    float healthPercent = health / 100.0f;
                    float filledHeight = totalBarHeight * healthPercent; // height of current health
                    ImGui::GetForegroundDrawList()->AddRectFilled(
                        ImVec2(barX, barTopY),
                        ImVec2(barX + barWidth, barBottomY - filledHeight),
                        ImColor(50, 50, 50, 200) // semi-transparent grey
                    );

                    // Optional outline
                    ImGui::GetForegroundDrawList()->AddRect(
                        ImVec2(barX, barTopY),
                        ImVec2(barX + barWidth, barBottomY),
                        ImColor(100, 100, 100)
                    );
                }
            }
        }
    }
}

void Esp::drawSettings()
{
    ImGui::Checkbox("Enable Esp", &enabled);

    if (enabled) {
        ImGui::Checkbox("Show Debug Menu", &showDebugMenu);
        ImGui::Checkbox("Show Name", &showName);
        ImGui::Checkbox("Show Health", &showHealth);
    }
}

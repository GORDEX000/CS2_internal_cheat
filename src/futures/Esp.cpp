#include "Esp.h"

uintptr_t Esp::client = (uintptr_t)GetModuleHandle(L"client.dll");

void Esp::drawESP()
{

    if (enabled) {

        float(*ViewMatrix)[4][4] = (float(*)[4][4])(client + Offsets::dwViewMatrix);

        auto localPawn = *(uintptr_t*)(client + Offsets::dwLocalPlayerPawn);
        if (!localPawn)
            return;

        auto entityList = *(uintptr_t*)(client + Offsets::dwEntityList);

        for (int i = 1; i < 64; i++) {
            uintptr_t list_entry1 = *(uintptr_t*)(entityList + (8 * (i & 0x7FFF) >> 9) + 16);
            if (!list_entry1)
                continue;

            uintptr_t playerController = *(uintptr_t*)(list_entry1 + 120 * (i & 0x1FF));
            if (!playerController)
                continue;

            uint32_t playerPawn = *(uint32_t*)(playerController + Offsets::m_hPlayerPawn);
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

            auto localTeam = *(int*)(localPawn + Offsets::m_iTeamNum);

            // skip teammates
            int playerTeam = *(int*)(pCSPlayerPawnPtr + Offsets::m_iTeamNum);
            if (playerTeam == localTeam)
                continue;

            // Name
            char* namePtr = *(char**)(playerController + Offsets::m_sSanitizedPlayerName);
            std::string entityName = namePtr ? std::string(namePtr) : "Unknown";

            // health
            int health = *(int*)(pCSPlayerPawnPtr + Offsets::m_iHealth);
            if (!health || health > 100)
                continue;

            // armor
            int armor = *(int*)(pCSPlayerPawnPtr + Offsets::m_ArmorValue);

            // pos
            Vec3 feetpos = *(Vec3*)(pCSPlayerPawnPtr + Offsets::m_vOldOrigin);
            Vec3 headpos = { feetpos.x + 0.0f, feetpos.y + 0.0f, feetpos.z + 73.0f };

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

                float barPadding = 2.0f;
                float barWidth = 7.0f;

                // Health bar
                if (showHealth) {
                    Esp::drawVerticalBar(
                        feet.x - width / 2 - barWidth, // barX
                        feet.y,
                        fabsf(feet.y - head.y),
                        barWidth,
                        health, 100,
                        ImColor(255, 0, 0),
                        ImColor(0, 255, 0),
                        ImColor(50, 50, 50, 200),
                        ImColor(100, 100, 100),
                        ImColor(255, 255, 255),
                        barPadding
                    );
                }

                // Armor bar
                if (showArmor) {
                    Esp::drawVerticalBar(
                        feet.x - width / 2 - barWidth - barWidth - barPadding, // barX (next to health)
                        feet.y,
                        fabsf(feet.y - head.y),
                        barWidth,
                        armor, 100,
                        ImColor(196, 76, 2),
                        ImColor(0, 0, 255),
                        ImColor(50, 50, 50, 200),
                        ImColor(100, 100, 100),
                        ImColor(0, 200, 255),
                        barPadding
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
        ImGui::Checkbox("Show Armor", &showArmor);
    }
}

void Esp::drawVerticalBar(float barX, float barBottomY, float totalHeight, float barWidth, int value, int maxValue,
    ImColor topColor, ImColor bottomColor, ImColor bgColor, ImColor outlineColor, ImColor textColor, float padding)
{
    // Adjust barX for padding
    barX += padding;

    int steps = (int)totalHeight;
    for (int i = 0; i < steps; i++) {
        float t = (float)i / steps;
        ImU32 col = ImColor(
            (int)((1.0f - t) * topColor.Value.x * 255 + t * bottomColor.Value.x * 255),
            (int)((1.0f - t) * topColor.Value.y * 255 + t * bottomColor.Value.y * 255),
            (int)((1.0f - t) * topColor.Value.z * 255 + t * bottomColor.Value.z * 255)
        );
        ImGui::GetForegroundDrawList()->AddRectFilled(
            ImVec2(barX, barBottomY - i - 1),
            ImVec2(barX + barWidth, barBottomY - i),
            col
        );
    }

    // Grey overlay for missing value
    float percent = value / (float)maxValue;
    float filledHeight = totalHeight * percent;
    ImGui::GetForegroundDrawList()->AddRectFilled(
        ImVec2(barX, barBottomY - totalHeight),
        ImVec2(barX + barWidth, barBottomY - filledHeight),
        bgColor
    );

    // Outline of full bar
    ImGui::GetForegroundDrawList()->AddRect(
        ImVec2(barX, barBottomY - totalHeight),
        ImVec2(barX + barWidth, barBottomY),
        outlineColor
    );

    // Outline of current filled portion
    ImGui::GetForegroundDrawList()->AddRect(
        ImVec2(barX, barBottomY - filledHeight),
        ImVec2(barX + barWidth, barBottomY),
        textColor
    );

    // Text that follows current value, centered
    std::string text = std::to_string(value);
    ImVec2 textSize = ImGui::CalcTextSize(text.c_str());
    float textX = barX + (barWidth - textSize.x) / 2;
    float textY = barBottomY - filledHeight - textSize.y - 2;

    ImGui::GetForegroundDrawList()->AddText(
        ImVec2(textX, textY),
        textColor,
        text.c_str()
    );

}

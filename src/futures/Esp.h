#pragma once
#include "../includes.h"

class Esp
{
public:
	void drawESP();
	void drawSettings();

private:
	void drawVerticalBar(float barX, float barBottomY, float totalHeight, float barWidth, int value, int maxValue,
		ImColor topColor, ImColor bottomColor, ImColor bgColor = ImColor(50, 50, 50, 200), ImColor outlineColor = ImColor(100, 100, 100), ImColor textColor = ImColor(255, 255, 255), float padding = 2.0f);

	static uintptr_t client;

	bool enabled = true;
	bool showName = true;
	bool showHealth = true;
	bool showArmor = true;

	bool showDebugMenu = false;
};


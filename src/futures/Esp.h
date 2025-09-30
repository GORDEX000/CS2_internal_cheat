#pragma once
#include "../includes.h"


class Esp
{
public:
	void drawESP();
	void drawSettings();

private:
	void drawVerticalBar(float barX, float barBottomY, float totalHeight, float barWidth, int value, int maxValue,
		ImColor topColor, ImColor bottomColor, ImColor bgColor, ImColor outlineColor, ImColor textColor, float padding);

	static uintptr_t client;

	bool enabled = true;
	bool showName = true;
	bool showHealth = true;
	bool showArmor = true;

	bool showDebugMenu = false;
};


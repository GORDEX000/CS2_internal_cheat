#pragma once
#include "../includes.h"

class Esp
{
public:
	void drawESP();
	void drawSettings();

private:
	static uintptr_t client;

	bool enabled = true;
	bool showName = true;
	bool showHealth = true;
	bool showDebugMenu = false;
};


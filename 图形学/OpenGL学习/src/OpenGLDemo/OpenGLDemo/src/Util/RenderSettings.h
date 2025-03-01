#pragma once
#include "UtilTemplate.h"

#define RSI (RenderSettings::GetInstance())

class RenderSettings : public Instance<RenderSettings>
{
public:
	int ViewportWidth = 800;
	int ViewportHeight = 600;
	int ViewportX = 0;
	int ViewportY = 0;
};


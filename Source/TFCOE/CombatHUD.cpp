// Created by Kairo Forge


#include "CombatHUD.h"

bool UCombatHUD::CheckPriorModeActive(const EUI_PlayerMode IncomingMode) const
{
	if (CurrentUI_Mode == IncomingMode)
	{
		return true;
	}
	
	return false;
}

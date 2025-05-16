// Fill out your copyright notice in the Description page of Project Settings.


#include "PropWithSpawnRules.h"

#include "PropSpawnRules.h"
#include "RoomSpawnRules.h"

bool UPropWithSpawnRules::Evaluate(const ULevelData* LevelData)
{
	if (Rules.Num() == 0)return true;
	for (const auto rule : Rules)
	{
		if (rule == nullptr)continue;
		if (!rule->CheckRules(LevelData))return false;
	}
	return true;
}

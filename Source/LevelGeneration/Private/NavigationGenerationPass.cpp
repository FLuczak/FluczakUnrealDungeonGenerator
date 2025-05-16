// Fill out your copyright notice in the Description page of Project Settings.


#include "NavigationGenerationPass.h"

#include "NavigationSystem.h"

void UNavigationGenerationPass::Generate(ULevelData* Data)
{
    if (UNavigationSystemV1* NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld()))
    {
        NavSystem->Build();
    }
}

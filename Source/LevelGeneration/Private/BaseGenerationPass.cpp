


#include "BaseGenerationPass.h"


// Sets default values for this component's properties
UBaseGenerationPass::UBaseGenerationPass()
{
	PrimaryComponentTick.bCanEverTick = true;

}

UBaseGenerationPass::~UBaseGenerationPass()
{

}

void UBaseGenerationPass::Generate(ULevelData* Data)
{

}

void UBaseGenerationPass::SetSeed(int32 NewSeed)
{
	Seed = NewSeed;
	SetupRNG();
}

void UBaseGenerationPass::SetupRNG()
{
	RandomNumberGenerator = std::mt19937(Seed);
}


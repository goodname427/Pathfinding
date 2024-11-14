#include "GameStage.h"

bool IGameStage::IsSameStage(const TSharedPtr<IGameStage>& Stage1, const TSharedPtr<IGameStage>& Stage2)
{
	if (!Stage1.IsValid() || !Stage2.IsValid())
		return false;

	return Stage1->GetStageName() == Stage2->GetStageName();
}

bool IGameStage::IsSameStage(const TSharedPtr<IGameStage>& Stage, FName StageName)
{
	return Stage.IsValid() && Stage->GetStageName() == StageName;
}

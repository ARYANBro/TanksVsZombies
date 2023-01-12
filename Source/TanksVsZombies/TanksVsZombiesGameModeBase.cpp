// Copyright Epic Games, Inc. All Rights Reserved.


#include "TanksVsZombiesGameModeBase.h"

void ATanksVsZombiesGameModeBase::OnPlayerDied()
{
    OnPlayerDiedEvent.Broadcast();
}

void ATanksVsZombiesGameModeBase::AddOnPlayerDiedDelegate(const FStopRenderingThreadDelegate& Delegate)
{
    OnPlayerDiedEvent.Add(Delegate);
}

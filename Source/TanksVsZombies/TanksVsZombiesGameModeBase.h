// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TanksVsZombiesGameModeBase.generated.h"

/**
 * 
 */
DECLARE_EVENT(ATanksVsZombiesGameModeBase, FOnPlayerDiedEventSignature)

UCLASS()
class TANKSVSZOMBIES_API ATanksVsZombiesGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	void AddOnPlayerDiedDelegate(const FStopRenderingThreadDelegate& Delegate);
private:
	UFUNCTION()
	void OnPlayerDied();

private:
	FOnPlayerDiedEventSignature OnPlayerDiedEvent;
};

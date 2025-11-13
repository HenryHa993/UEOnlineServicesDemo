// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "EOSPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class ONLINEMENUTUTORIAL_API AEOSPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AEOSPlayerController();

protected:
	virtual void BeginPlay() override;

	void Login();

	void OnLoginCompleted(int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error);

	// Delegate to bind callback event for login
	FDelegateHandle LoginDelegateHandle;
};

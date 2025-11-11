// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OnlineSubsystem.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "MultiplayerSessionsSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class ONLINEMENUTUTORIAL_API UMultiplayerSessionsSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UMultiplayerSessionsSubsystem();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual void Deinitialize() override;

	void PrintString(const FString& Value);

	UFUNCTION(BlueprintCallable)
	void CreateServer(FString ServerName);

	UFUNCTION(BlueprintCallable)
	void FindServer(FString ServerName);

	void OnSessionCreatedComplete(FName sessionName, bool wasSuccessful);

	void OnDestroySessionComplete(FName sessionName, bool wasSuccessful);

	IOnlineSubsystem* OnlineSubsystem;
	IOnlineSessionPtr SessionInterface;

	bool CreateSessionAfterDestroy = false;
	
	FString DestroyServerName;
	FString ServerNameToFind;
	
	TSharedPtr<FOnlineSessionSearch> SessionSearch;

	void OnFindSessionsComplete(bool wasSuccessful);

	void OnJoinSessionComplete(FName sessionName, EOnJoinSessionCompleteResult::Type result);

	FName SessionName;
};

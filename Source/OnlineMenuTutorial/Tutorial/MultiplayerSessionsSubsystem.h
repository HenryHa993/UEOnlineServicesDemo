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
	void Login();
	void OnLoginComplete(int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId,
	const FString& Error);

	// Create server functions
	UFUNCTION(BlueprintCallable)
	void CreateSession(FString ServerName);
	
	void OnSessionCreatedComplete(FName sessionName, bool wasSuccessful);

	// Find server functions
	UFUNCTION(BlueprintCallable)
	void FindServer(FString ServerName);
	
	void OnFindSessionsComplete(bool wasSuccessful);

	// Destroy session functions
	void OnDestroySessionComplete(FName sessionName, bool wasSuccessful);

	// Join session functions
	void OnJoinSessionComplete(FName sessionName, EOnJoinSessionCompleteResult::Type result);

public:
	FName SessionName;
	
	IOnlineSubsystem* OnlineSubsystem;
    IOnlineSessionPtr SessionInterface;

    bool CreateSessionAfterDestroy = false;
    
    FString DestroyServerName;
    FString ServerNameToFind;

	TSharedPtr<FOnlineSessionSearch> SessionSearch;

	bool IsLoggedIn;
};

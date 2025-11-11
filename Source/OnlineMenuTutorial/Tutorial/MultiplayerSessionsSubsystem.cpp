// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiplayerSessionsSubsystem.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Online/OnlineSessionNames.h"

UMultiplayerSessionsSubsystem::UMultiplayerSessionsSubsystem() :
	ServerNameToFind(""),
	SessionName("Multiplayer Session Name"),
	CreateSessionAfterDestroy(false),
	DestroyServerName("")
{
	//PrintString("MSS Constructor");
}

void UMultiplayerSessionsSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	//PrintString("MSS Init");

	// Get online subsystem
	OnlineSubsystem = IOnlineSubsystem::Get();
	if (OnlineSubsystem)
	{
		SessionInterface = OnlineSubsystem->GetSessionInterface();
		if (SessionInterface.IsValid()) // This is actually a shared pointer
		{
			// Valid!
			FString onlineSubsystemName = OnlineSubsystem->GetSubsystemName().ToString();
			PrintString(FString::Printf(TEXT("Valid Online Subsystem: %s"), *onlineSubsystemName));

			// Binding a function to the delegate which fires when the creation of a session is complete
			// We are delivering the address of the function
			SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this,
				&UMultiplayerSessionsSubsystem::OnSessionCreatedComplete);
			SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this,
				&UMultiplayerSessionsSubsystem::OnDestroySessionComplete);
			SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this,
				&UMultiplayerSessionsSubsystem::OnFindSessionsComplete);
			SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this,
				&UMultiplayerSessionsSubsystem::OnJoinSessionComplete);
		}
	}
}

void UMultiplayerSessionsSubsystem::Deinitialize()
{
	Super::Deinitialize();
	//UE_LOG(LogTemp, Warning, TEXT("MSS Deinit"));
}

void UMultiplayerSessionsSubsystem::PrintString(const FString& Value)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, Value);
	}
}

void UMultiplayerSessionsSubsystem::CreateServer(FString ServerName)
{
	PrintString("CreateServer: Started");
	// Check if name is empty
	if (ServerName.IsEmpty())
	{
		PrintString("CreateServer: Server name is empty.");
		return;
	}

	// Check session exists, and destroy if so
	FNamedOnlineSession* namedOnlineSession = SessionInterface->GetNamedSession(SessionName);
	if (namedOnlineSession)
	{
		// This is also an async operation
		CreateSessionAfterDestroy = true;
		DestroyServerName = ServerName;
		SessionInterface->DestroySession(SessionName);
		return;
	}

	// Set up online session settings
	FOnlineSessionSettings sessionSettings;
	sessionSettings.bAllowJoinInProgress = true; // Allow join after session start
	sessionSettings.bIsDedicated = false; // Whether it is a dedicated server
	sessionSettings.bShouldAdvertise = true; // Whether we want the server to be advertised
	sessionSettings.NumPublicConnections = 2; // Max players
	sessionSettings.bUseLobbiesIfAvailable = true; // Lobbies API for Steam, just needs to be set to true
	sessionSettings.bUsesPresence = true; // Whether player's presence are available to the API, must be set to true also
	sessionSettings.bAllowJoinViaPresence = true;

	bool isLan = true;
	if (OnlineSubsystem->GetSubsystemName().ToString() == FString("NULL"))
	{
		PrintString("CreateServer: isLan = true.");
		isLan = true; // Whether this is LAN or not, change depend on if using Steam or NULL
	}
	else
	{
		PrintString("CreateServer: isLan = false.");
		isLan = false;
	}
	
	sessionSettings.bIsLANMatch = isLan;
	// Takes in a key-value pair
	// Key : FName
	// Value : Name
	// How you advertise it
	sessionSettings.Set(FName("SERVER_NAME"), ServerName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	// 0, as we are creating the session, session name is used to differentiate sessions
	// This is an async process, we must wait for the call back
	SessionInterface->CreateSession(0, SessionName, sessionSettings);
}

void UMultiplayerSessionsSubsystem::FindServer(FString ServerName)
{
	PrintString("FindServer: Started");

	// Check empty server name
	if (ServerName.IsEmpty())
	{
		PrintString("FindServer: Server name cannot be empty.");
		return;
	}

	// Session search settings
	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	bool isLan = false;
	if (IOnlineSubsystem::Get()->GetSubsystemName() == "NULL")
	{
		PrintString("FindServer: isLan = true.");
		isLan = true;
	}
	else
	{
		PrintString("FindServer: isLan = false.");
		isLan = false;
	}
	SessionSearch->bIsLanQuery = isLan;
	SessionSearch->MaxSearchResults = 9999;
	SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals); // Make sure to include Online/OnlineSessionNames

	ServerNameToFind = ServerName;
	
	// This is an asynchronous call, similar to on create and on destroy sessions.
	SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
}

void UMultiplayerSessionsSubsystem::OnSessionCreatedComplete(FName sessionName, bool wasSuccessful)
{
	PrintString(FString::Printf(TEXT("OnSessionCreatedComplete: wasSuccessful = %d"), wasSuccessful));

	if (wasSuccessful)
	{
		// Jump server to a new level and launch it as a listen server
		GetWorld()->ServerTravel("/Game/ThirdPerson/Lvl_ThirdPerson?listen");
		APlayerController* playerController = GetWorld()->GetFirstPlayerController();
		playerController->SetShowMouseCursor(false);
		FInputModeGameOnly inputMode;
		playerController->SetInputMode(inputMode);
	}
}

void UMultiplayerSessionsSubsystem::OnDestroySessionComplete(FName sessionName, bool wasSuccessful)
{
	PrintString("OnDestroySessionComplete: Started");
	if (CreateSessionAfterDestroy)
	{
		CreateSessionAfterDestroy = false;
		CreateServer(DestroyServerName);
	}
}

void UMultiplayerSessionsSubsystem::OnFindSessionsComplete(bool wasSuccessful)
{
	PrintString("OnFindSessionsComplete: Started");

	if (!wasSuccessful)
	{
		PrintString("OnFindSessionComplete: Unsuccessful");
		return;
	}
	if (ServerNameToFind.IsEmpty())
	{
		PrintString("OnFindSessionComplete: No Servers Found");
		return;
	}
	
	TArray<FOnlineSessionSearchResult> results = SessionSearch->SearchResults;
	FOnlineSessionSearchResult* correctResult = 0;
	
	if (results.Num() > 0)
	{
		FString msg = FString::Printf(TEXT("OnFindSessionComplete: %d sessions found."),results.Num());
		PrintString(msg);

		for (FOnlineSessionSearchResult result : results)
		{
			if (result.IsValid())
			{
				FString serverName = "No-name";
				result.Session.SessionSettings.Get(FName("SERVER_NAME"), serverName);

				if (serverName.Equals(ServerNameToFind))
				{
					correctResult = &result;
					FString msg2 = FString::Printf(TEXT("ServerName: %s"), *serverName);
					PrintString(msg2);
					break;
				}
			}
		}
		
		if (correctResult)
		{
			PrintString("OnFindSessionComplete: Joining Session.");
			SessionInterface->JoinSession(0, SessionName, *correctResult);
		}
		else
		{
			PrintString("OnFindSessionComplete: Could not find server.");
		}
	}
	else
	{
		PrintString("OnFindSessionComplete: No sessions found");
	}
}

void UMultiplayerSessionsSubsystem::OnJoinSessionComplete(FName sessionName, EOnJoinSessionCompleteResult::Type result)
{
	if (result == EOnJoinSessionCompleteResult::Success)
	{
		PrintString("OnJoinSessionComplete: Success");
		FString address = "";
		bool success = SessionInterface->GetResolvedConnectString(SessionName, address);

		if (success)
		{
			PrintString(FString::Printf(TEXT("Address: %s"), *address));
			APlayerController* playerController = GetGameInstance()->GetFirstLocalPlayerController();

			if (playerController)
			{
				playerController->ClientTravel(address, TRAVEL_Absolute);
			}
		}
		else
		{
			PrintString("OnJoinSessionComplete: GetResolvedConnectString = false.");
		}
	}
	else
	{
		PrintString("OnJoinSessionComplete: Unsuccessful");
	}
}

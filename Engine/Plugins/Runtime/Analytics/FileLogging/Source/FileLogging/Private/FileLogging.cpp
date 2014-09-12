// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "FileLoggingPrivatePCH.h"

DEFINE_LOG_CATEGORY_STATIC(LogFileLoggingAnalytics, Display, All);

IMPLEMENT_MODULE( FAnalyticsFileLogging, FileLogging )

void FAnalyticsFileLogging::StartupModule()
{
	FileLoggingProvider = MakeShareable(new FAnalyticsProviderFileLogging());
}

void FAnalyticsFileLogging::ShutdownModule()
{
	if (FileLoggingProvider.IsValid())
	{
		FileLoggingProvider->EndSession();
	}
}

TSharedPtr<IAnalyticsProvider> FAnalyticsFileLogging::CreateAnalyticsProvider(const FAnalytics::FProviderConfigurationDelegate& GetConfigValue) const
{
	return FileLoggingProvider;
}

// Provider

FAnalyticsProviderFileLogging::FAnalyticsProviderFileLogging() :
	bHasSessionStarted(false),
	bHasWrittenFirstEvent(false),
	FileArchive(nullptr)
{
	FileArchive = nullptr;
	AnalyticsFilePath = FPaths::GameSavedDir() + TEXT("Analytics/");
	UserId = FPlatformMisc::GetUniqueDeviceId();
}

FAnalyticsProviderFileLogging::~FAnalyticsProviderFileLogging()
{
	if (bHasSessionStarted)
	{
		EndSession();
	}
}

bool FAnalyticsProviderFileLogging::StartSession(const TArray<FAnalyticsEventAttribute>& Attributes)
{
	if (bHasSessionStarted)
	{
		EndSession();
	}
	SessionId = UserId + TEXT("-") + FDateTime::Now().ToString();
	const FString FileName = AnalyticsFilePath + SessionId + TEXT(".analytics");
	// Close the old file and open a new one
	FileArchive = IFileManager::Get().CreateFileWriter(*FileName);
	if (FileArchive != nullptr)
	{
		FileArchive->Logf(TEXT("{"));
		FileArchive->Logf(TEXT("\t\"sessionId\" : \"%s\","), *SessionId);
		FileArchive->Logf(TEXT("\t\"userId\" : \"%s\","), *UserId);
		FileArchive->Logf(TEXT("\t\"events\" : ["));
		bHasSessionStarted = true;
	}
	else
	{
		UE_LOG(LogFileLoggingAnalytics, Warning, TEXT("FAnalyticsProviderFileLogging::StartSession failed to create file to log analytics events to"));
	}
	return bHasSessionStarted;
}

void FAnalyticsProviderFileLogging::EndSession()
{
	if (FileArchive != nullptr)
	{
		FileArchive->Logf(TEXT("\t]"));
		FileArchive->Logf(TEXT("}"));
		FileArchive->Flush();
		FileArchive->Close();
		delete FileArchive;
		FileArchive = nullptr;
	}
	bHasWrittenFirstEvent = false;
	bHasSessionStarted = false;
}

void FAnalyticsProviderFileLogging::FlushEvents()
{
	if (FileArchive != nullptr)
	{
		FileArchive->Flush();
	}
}

void FAnalyticsProviderFileLogging::SetUserID(const FString& InUserID)
{
	if (!bHasSessionStarted)
	{
		UserId = InUserID;
	}
	else
	{
		// Log that we shouldn't switch users during a session
		UE_LOG(LogFileLoggingAnalytics, Warning, TEXT("FAnalyticsProviderFileLogging::SetUserID called while a session is in progress. Ignoring."));
	}
}

FString FAnalyticsProviderFileLogging::GetUserID() const
{
	return UserId;
}

FString FAnalyticsProviderFileLogging::GetSessionID() const
{
	return SessionId;
}

bool FAnalyticsProviderFileLogging::SetSessionID(const FString& InSessionID)
{
	if (!bHasSessionStarted)
	{
		SessionId = InSessionID;
	}
	else
	{
		// Log that we shouldn't switch session ids during a session
		UE_LOG(LogFileLoggingAnalytics, Warning, TEXT("FAnalyticsProviderFileLogging::SetSessionID called while a session is in progress. Ignoring."));
	}
	return !bHasSessionStarted;
}

void FAnalyticsProviderFileLogging::RecordEvent(const FString& EventName, const TArray<FAnalyticsEventAttribute>& Attributes)
{
	if (bHasSessionStarted)
	{
		check(FileArchive != nullptr);

		if (bHasWrittenFirstEvent)
		{
			FileArchive->Logf(TEXT(","));
		}
		bHasWrittenFirstEvent = true;

		FileArchive->Logf(TEXT("\t\t{"));
		FileArchive->Logf(TEXT("\t\t\t\"eventName\" : \"%s\""), *EventName);
		if (Attributes.Num() > 0)
		{
			FileArchive->Logf(TEXT(",\t\t\t\"attributes\" : ["));
			bool bHasWrittenFirstAttr = false;
			// Write out the list of attributes as an array of attribute objects
			for (auto Attr : Attributes)
			{
				if (bHasWrittenFirstAttr)
				{
					FileArchive->Logf(TEXT("\t\t\t,"));
				}
				FileArchive->Logf(TEXT("\t\t\t{"));
				FileArchive->Logf(TEXT("\t\t\t\t\"name\" : \"%s\","), *Attr.AttrName);
				FileArchive->Logf(TEXT("\t\t\t\t\"value\" : \"%s\""), *Attr.AttrValue);
				FileArchive->Logf(TEXT("\t\t\t}"));
				bHasWrittenFirstAttr = true;
			}
			FileArchive->Logf(TEXT("\t\t\t]"));
		}
		FileArchive->Logf(TEXT("\t\t}"));
	}
	else
	{
		UE_LOG(LogFileLoggingAnalytics, Warning, TEXT("FAnalyticsProviderFileLogging::RecordEvent called before StartSession. Ignoring."));
	}
}

void FAnalyticsProviderFileLogging::RecordItemPurchase(const FString& ItemId, const FString& Currency, int PerItemCost, int ItemQuantity)
{
	if (bHasSessionStarted)
	{
		check(FileArchive != nullptr);

		if (bHasWrittenFirstEvent)
		{
			FileArchive->Logf(TEXT("\t\t,"));
		}
		bHasWrittenFirstEvent = true;

		FileArchive->Logf(TEXT("\t\t{"));
		FileArchive->Logf(TEXT("\t\t\t\"eventName\" : \"recordItemPurchase\","));

		FileArchive->Logf(TEXT("\t\t\t\"attributes\" :"));
		FileArchive->Logf(TEXT("\t\t\t["));

		FileArchive->Logf(TEXT("\t\t\t\t{ \"name\" : \"itemId\", \t\"value\" : \"%s\" },"), *ItemId);
		FileArchive->Logf(TEXT("\t\t\t\t{ \"name\" : \"currency\", \t\"value\" : \"%s\" },"), *Currency);
		FileArchive->Logf(TEXT("\t\t\t\t{ \"name\" : \"perItemCost\", \t\"value\" : \"%d\" },"), PerItemCost);
		FileArchive->Logf(TEXT("\t\t\t\t{ \"name\" : \"itemQuantity\", \t\"value\" : \"%d\" }"), ItemQuantity);

		FileArchive->Logf(TEXT("\t\t\t]"));

		FileArchive->Logf(TEXT("\t\t}"));
	}
	else
	{
		UE_LOG(LogFileLoggingAnalytics, Warning, TEXT("FAnalyticsProviderFileLogging::RecordItemPurchase called before StartSession. Ignoring."));
	}
}

void FAnalyticsProviderFileLogging::RecordCurrencyPurchase(const FString& GameCurrencyType, int GameCurrencyAmount, const FString& RealCurrencyType, float RealMoneyCost, const FString& PaymentProvider)
{
	if (bHasSessionStarted)
	{
		check(FileArchive != nullptr);

		if (bHasWrittenFirstEvent)
		{
			FileArchive->Logf(TEXT("\t\t,"));
		}
		bHasWrittenFirstEvent = true;

		FileArchive->Logf(TEXT("\t\t{"));
		FileArchive->Logf(TEXT("\t\t\t\"eventName\" : \"recordCurrencyPurchase\","));

		FileArchive->Logf(TEXT("\t\t\t\"attributes\" :"));
		FileArchive->Logf(TEXT("\t\t\t["));

		FileArchive->Logf(TEXT("\t\t\t\t{ \"name\" : \"gameCurrencyType\", \t\"value\" : \"%s\" },"), *GameCurrencyType);
		FileArchive->Logf(TEXT("\t\t\t\t{ \"name\" : \"gameCurrencyAmount\", \t\"value\" : \"%d\" },"), GameCurrencyAmount);
		FileArchive->Logf(TEXT("\t\t\t\t{ \"name\" : \"realCurrencyType\", \t\"value\" : \"%s\" },"), *RealCurrencyType);
		FileArchive->Logf(TEXT("\t\t\t\t{ \"name\" : \"realMoneyCost\", \t\"value\" : \"%f\" },"), RealMoneyCost);
		FileArchive->Logf(TEXT("\t\t\t\t{ \"name\" : \"paymentProvider\", \t\"value\" : \"%s\" }"), *PaymentProvider);

		FileArchive->Logf(TEXT("\t\t\t]"));

		FileArchive->Logf(TEXT("\t\t}"));
	}
	else
	{
		UE_LOG(LogFileLoggingAnalytics, Warning, TEXT("FAnalyticsProviderFileLogging::RecordCurrencyPurchase called before StartSession. Ignoring."));
	}
}

void FAnalyticsProviderFileLogging::RecordCurrencyGiven(const FString& GameCurrencyType, int GameCurrencyAmount)
{
	if (bHasSessionStarted)
	{
		check(FileArchive != nullptr);

		if (bHasWrittenFirstEvent)
		{
			FileArchive->Logf(TEXT("\t\t,"));
		}
		bHasWrittenFirstEvent = true;

		FileArchive->Logf(TEXT("\t\t{"));
		FileArchive->Logf(TEXT("\t\t\t\"eventName\" : \"recordCurrencyGiven\","));

		FileArchive->Logf(TEXT("\t\t\t\"attributes\" :"));
		FileArchive->Logf(TEXT("\t\t\t["));

		FileArchive->Logf(TEXT("\t\t\t\t{ \"name\" : \"gameCurrencyType\", \t\"value\" : \"%s\" },"), *GameCurrencyType);
		FileArchive->Logf(TEXT("\t\t\t\t{ \"name\" : \"gameCurrencyAmount\", \t\"value\" : \"%d\" }"), GameCurrencyAmount);

		FileArchive->Logf(TEXT("\t\t\t]"));

		FileArchive->Logf(TEXT("\t\t}"));
	}
	else
	{
		UE_LOG(LogFileLoggingAnalytics, Warning, TEXT("FAnalyticsProviderFileLogging::RecordCurrencyGiven called before StartSession. Ignoring."));
	}
}

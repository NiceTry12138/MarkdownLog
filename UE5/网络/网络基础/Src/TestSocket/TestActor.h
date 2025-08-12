// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "HAL/Runnable.h"

#include "IPAddress.h"
#include "Sockets.h"

#include "TestActor.generated.h"

//class FSocket;

class FRunnableThread;

/**
* Helper class inhibiting screen saver by e.g. moving the mouse by 0 pixels every 50 seconds.
*/
class FSocketListener : public FRunnable
{
public:
	FSocketListener(FInternetAddr* InAddr)
		: bEnabled(true), Addr(InAddr)
	{}

protected:

	bool Init() override;

	void Stop() override
	{
		bEnabled = false;
		FPlatformMisc::MemoryBarrier();
	}

	uint32 Run() override;

	void InitSocket();
	bool AcceptClient();
	void ProcessClientData();

protected:
	FInternetAddr* Addr = nullptr;
	FSocket* Socket = nullptr;
	FSocket* ClientSocket = nullptr;
	bool bEnabled = true;
};


UCLASS(BlueprintType)
class EMPTY53_API ATestActor : public AActor
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	bool BeginConnect();

	UFUNCTION(BlueprintCallable)
	bool SendMessage(const FString& Msg);

public:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString Ip;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 Port;


	FSocket* Host;
	TSharedPtr<FInternetAddr> Addr;
	FRunnableThread* Thread = nullptr;
};

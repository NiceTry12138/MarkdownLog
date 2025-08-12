// Fill out your copyright notice in the Description page of Project Settings.


#include "TestActor.h"
#include "Interfaces/IPv4/IPv4Address.h"
#include "SocketSubsystem.h"
#include "SocketTypes.h"

bool ATestActor::BeginConnect()
{
	ISocketSubsystem* System = ISocketSubsystem::Get();
	Host = System->CreateSocket(NAME_Stream, TEXT("default"), false);
	Host->SetReuseAddr(true);

	uint32 IPAddr;
	Addr->GetIp(IPAddr);
	auto OutPort = Addr->GetPort();

	if (!Host->Connect(*Addr))
	{
		ESocketErrors ErrorCode = System->GetLastErrorCode();
		UE_LOG(LogTemp, Log, TEXT("Host Connect Faild %s"), System->GetSocketError(ErrorCode));
		return false;
	}

	return true;
}

bool ATestActor::SendMessage(const FString& Msg)
{
	auto Data = Msg.GetCharArray().GetData();

	int32 Size = FCString::Strlen(Data) + 1;
	int32 Send = 0;

	if (Host->Send((uint8*)TCHAR_TO_UTF8(Data), Size, Send))
	{
		UE_LOG(LogTemp, Log, TEXT("Send Message Success"));
		return true;
	}

	UE_LOG(LogTemp, Log, TEXT("Send Message Faild"));
	return false;
}

void ATestActor::BeginPlay()
{
	Super::BeginPlay();

	FIPv4Address IpAddr;
	FIPv4Address::Parse(Ip, IpAddr);
	ISocketSubsystem* System = ISocketSubsystem::Get();

	Addr = System->CreateInternetAddr().ToSharedPtr();
	Addr->SetIp(IpAddr.Value);
	Addr->SetPort(Port);

	Thread = FRunnableThread::Create(new FSocketListener(Addr.Get()), TEXT("Listener"));
}

void ATestActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (Thread)
	{
		Thread->Kill();
		Thread = nullptr;
	}
	ISocketSubsystem* System = ISocketSubsystem::Get();
	if (Host)
	{
		Host->Close();
		System->DestroySocket(Host);
		Host = nullptr;
	}
}

bool FSocketListener::Init()
{
	return true;
}

uint32 FSocketListener::Run()
{
	do
	{
		InitSocket();
	} while (Socket == nullptr);

	TArray<uint8> ReceiveData;
	uint8 Element = 0;
	int32 BytesRead(0);

	while (bEnabled)
	{
		// 接受客户端连接
		if (AcceptClient())
		{
			// 处理客户端数据
			ProcessClientData();
		}
	}
	return 0;
}

void FSocketListener::InitSocket()
{
	ISocketSubsystem* System = ISocketSubsystem::Get();
	Socket = System->CreateSocket(NAME_Stream, TEXT("default"), false);
	Socket->SetReuseAddr(true);
	if(!Socket->Bind(*Addr))
	{
		System->DestroySocket(Socket);
		Socket = nullptr;
		return;
	}

	Socket->Listen(5);
}

bool FSocketListener::AcceptClient()
{       
	// 检查是否有待处理的连接
	if (!Socket->Wait(ESocketWaitConditions::WaitForRead, FTimespan::FromSeconds(2)))
	{
		return false;
	}

	// 接受新连接
	ClientSocket = Socket->Accept(TEXT("ClientConnection"));
	if (!ClientSocket)
	{
		return false;
	}

	UE_LOG(LogTemp, Log, TEXT("New client connected"));
	return true;
}

void FSocketListener::ProcessClientData()
{
	TArray<uint8> ReceiveData;
	ReceiveData.SetNumUninitialized(1024);

	uint32 BytesRead = 0;

	// 接收数据
	while (ClientSocket->HasPendingData(BytesRead))
	{
		int32 Read = 0;
		if (ClientSocket->Recv(ReceiveData.GetData(), ReceiveData.Num(), Read))
		{
			if (Read > 0)
			{
				// 转换为字符串 (注意: 实际应用中需要处理二进制数据)
				FString ReceivedString = FString(UTF8_TO_TCHAR(reinterpret_cast<const char*>(ReceiveData.GetData())));
				UE_LOG(LogTemp, Warning, TEXT("Received: %s"), *ReceivedString.Left(Read));
			}
		}
	}

	// 关闭客户端连接
	ClientSocket->Close();
	ISocketSubsystem::Get()->DestroySocket(ClientSocket);
	ClientSocket = nullptr;
}

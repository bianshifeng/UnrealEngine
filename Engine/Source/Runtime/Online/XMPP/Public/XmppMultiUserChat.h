// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once

/**
 * Id of a chat room
 */
typedef FString FXmppRoomId;

/**
 * Role of a chat room member
 */
namespace EXmppChatMemberRole
{
	enum Type
	{
		Owner,
		Moderator,
		Member,
		None,
		Outcast
	};
}

/**
 * Member of a chat room
 */
class FXmppChatMember
{
public:
	FXmppChatMember()
		: Affiliation(EXmppChatMemberRole::Member)
	{}

	FString Nickname;
	FXmppUserJid MemberJid;
	FXmppUserPresence UserPresence;
	EXmppChatMemberRole::Type Affiliation;

	FString ToDebugString() const
	{
		return FString::Printf(TEXT("%s [%s] Role: %d"), *Nickname, *MemberJid.ToDebugString(), (int32)Affiliation);
	}
};

typedef TSharedPtr<FXmppChatMember, ESPMode::ThreadSafe> FXmppChatMemberPtr;
typedef TSharedRef<FXmppChatMember, ESPMode::ThreadSafe> FXmppChatMemberRef;

/**
 * Info for a joined/created chat room
 */
class FXmppRoomInfo
{
public:
	FXmppRoomInfo()
		: bIsPrivate(false)
	{}

	FXmppRoomId Id;
	FString OwnerId;
	FString Subject;
	bool bIsPrivate;

	FString ToDebugString() const
	{
		return FString::Printf(TEXT("%s Owner: %s Subj: %s Priv: %d"), *Id, *OwnerId, *Subject, bIsPrivate);
	}
};

/**
 * Info to configure a new room
 */
class FXmppRoomConfig
{
public:
	FXmppRoomConfig()
		: RoomName(TEXT("")),
		RoomDesc(TEXT("")),
		bIsPrivate(true),
		Password(TEXT("")),
		bIsPersistent(false),
		bAllowPublicSearch(false),
		bIsModerated(false),
		bIsMembersOnly(false),
		bAllowChangeSubject(false),
		MaxMsgHistory(0),
		RoomAnonymity(ERoomAnonymity::Semianonymous)
	{}

	enum class ERoomAnonymity
	{
		Semianonymous,
		Nonanonymous,
		Fullanonymous
	};

	static const FString ConvertRoomAnonymityToString(ERoomAnonymity InRoomAnonymity)
	{
		if (InRoomAnonymity == ERoomAnonymity::Nonanonymous)
		{
			return TEXT("nonanonymous");
		}
		else if (InRoomAnonymity == ERoomAnonymity::Semianonymous)
		{
			return TEXT("semianonymous");
		}
		else if (InRoomAnonymity == ERoomAnonymity::Fullanonymous)
		{
			return TEXT("fullanonymous");
		}
		else
		{
			return TEXT("INVALID ROOMANONYMITY ENUM");
		}
	}

	// <field type = "text-single" label = "Natural-Language Room Name" var = "muc#roomconfig_roomname">
	FString RoomName;
	// <field type = "text-single" label = "Short Description of Room" var = "muc#roomconfig_roomdesc">
	FString RoomDesc;
	// <field type = "boolean" label = "Password Required to Enter?" var = "muc#roomconfig_passwordprotectedroom">
	bool bIsPrivate;
	// <field type = "text-single" label = "Password" var = "muc#roomconfig_roomsecret">
	FString Password;
	// <field type = "boolean" label = "Make Room Persistent?" var = "muc#roomconfig_persistentroom">
	bool bIsPersistent;

private:
	// These fields are not used yet.  Move to public when hooking up
	// <field type = "boolean" label = "Make Room Publicly Searchable?" var = "muc#roomconfig_publicroom">
	bool bAllowPublicSearch;
	// <field type = "boolean" label = "Make Room Moderated?" var = "muc#roomconfig_moderatedroom">
	bool bIsModerated;
	// <field type = "boolean" label = "Make Room Members Only?" var = "muc#roomconfig_membersonly">
	bool bIsMembersOnly;
	// <field type = "boolean" label = "Allow Occupants to Change Subject?" var = "muc#roomconfig_changesubject">
	bool bAllowChangeSubject;
	// <field type = "text-single" label = "Maximum Number of History Messages Returned by Room" var = "muc#maxhistoryfetch">
	int32 MaxMsgHistory;
	// <field type = "list-single" label = "Room anonymity level:" var = "muc#roomconfig_anonymity">
	ERoomAnonymity RoomAnonymity;
	// <field type = "list-single" label = "Presence delivery logic" var = "tigase#presence_delivery_logic">
	// <field type = "boolean" label = "Enable filtering of presence (broadcasting presence only between selected groups" var = "tigase#presence_filtering">
	// <field type = "list-multi" label = "Affiliations for which presence should be delivered" var = "tigase#presence_filtered_affiliations">

	// Allow RoomConfigOp to use the defaults we don't expose yet for consumers to change
	friend class FXmppChatRoomConfigOp;
};

/**
 * Interface for creating/joining/chatting in a MUC
 */
class IXmppMultiUserChat
{
public:

	virtual ~IXmppMultiUserChat() {}

	virtual bool CreateRoom(const FXmppRoomId& RoomId, const FString& Nickname, const FXmppRoomConfig& RoomConfig = FXmppRoomConfig()) = 0;
	virtual bool ConfigureRoom(const FXmppRoomId& RoomId, const FXmppRoomConfig& RoomConfig) = 0;
	virtual bool JoinPublicRoom(const FXmppRoomId& RoomId, const FString& Nickname) = 0;
	virtual bool JoinPrivateRoom(const FXmppRoomId& RoomId, const FString& Nickname, const FString& Password) = 0;
	virtual bool RegisterMember(const FXmppRoomId& RoomId, const FString& Nickname) = 0;
	virtual bool UnregisterMember(const FXmppRoomId& RoomId, const FString& Nickname) = 0;
	virtual bool ExitRoom(const FXmppRoomId& RoomId) = 0;
	virtual bool SendChat(const FXmppRoomId& RoomId, const class FString& MsgBody) = 0;
	virtual void GetJoinedRooms(TArray<FXmppRoomId>& OutRooms) = 0;
	virtual bool RefreshRoomInfo(const FXmppRoomId& RoomId) = 0;
	virtual bool GetRoomInfo(const FXmppRoomId& RoomId, FXmppRoomInfo& OutRoomInfo) = 0;
	virtual bool GetMembers(const FXmppRoomId& RoomId, TArray<FXmppChatMemberRef>& OutMembers) = 0;
	virtual FXmppChatMemberPtr GetMember(const FXmppRoomId& RoomId, const FXmppUserJid& MemberJid) = 0;
	virtual bool GetLastMessages(const FXmppRoomId& RoomId, int32 NumMessages, TArray< TSharedRef<FXmppChatMessage> >& OutMessages) = 0;
	virtual void HandleMucPresence(const FXmppMucPresence& MemberPresence) = 0;

	DECLARE_MULTICAST_DELEGATE_FourParams(FOnXmppRoomCreateComplete, const TSharedRef<IXmppConnection>& /*Connection*/, bool /*bSuccess*/, const FXmppRoomId& /*RoomId*/, const FString& /*Error*/);
	/** @return room created delegate */
	virtual FOnXmppRoomCreateComplete& OnRoomCreated() = 0;

	DECLARE_MULTICAST_DELEGATE_FourParams(FOnXmppRoomConfigureComplete, const TSharedRef<IXmppConnection>& /*Connection*/, bool /*bSuccess*/, const FXmppRoomId& /*RoomId*/, const FString& /*Error*/);
	/** @return room configured delegate */
	virtual FOnXmppRoomConfigureComplete& OnRoomConfigured() = 0;

	DECLARE_MULTICAST_DELEGATE_FourParams(FOnXmppRoomInfoRefreshComplete, const TSharedRef<IXmppConnection>& /*Connection*/, bool /*bSuccess*/, const FXmppRoomId& /*RoomId*/, const FString& /*Error*/);
	/** @return room refresh delegate */
	virtual FOnXmppRoomInfoRefreshComplete& OnRoomInfoRefreshed() = 0;

	DECLARE_MULTICAST_DELEGATE_FourParams(FOnXmppRoomJoinPublicComplete, const TSharedRef<IXmppConnection>& /*Connection*/, bool /*bSuccess*/, const FXmppRoomId& /*RoomId*/, const FString& /*Error*/);
	/** @return public room joined delegate */
	virtual FOnXmppRoomJoinPublicComplete& OnJoinPublicRoom() = 0;

	DECLARE_MULTICAST_DELEGATE_FourParams(FOnXmppRoomJoinPrivateComplete, const TSharedRef<IXmppConnection>& /*Connection*/, bool /*bSuccess*/, const FXmppRoomId& /*RoomId*/, const FString& /*Error*/);
	/** @return private room joined delegate */
	virtual FOnXmppRoomJoinPrivateComplete& OnJoinPrivateRoom() = 0;

	DECLARE_MULTICAST_DELEGATE_FourParams(FOnXmppRoomExitComplete, const TSharedRef<IXmppConnection>& /*Connection*/, bool /*bSuccess*/, const FXmppRoomId& /*RoomId*/, const FString& /*Error*/);
	/** @return exit room delegate */
	virtual FOnXmppRoomExitComplete& OnExitRoom() = 0;

	DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnXmppRoomMemberJoin, const TSharedRef<IXmppConnection>& /*Connection*/, const FXmppRoomId& /*RoomId*/, const FXmppUserJid& /*UserJid*/);
	/** @return room member joined room delegate */
	virtual FOnXmppRoomMemberJoin& OnRoomMemberJoin() = 0;

	DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnXmppRoomMemberExit, const TSharedRef<IXmppConnection>& /*Connection*/, const FXmppRoomId& /*RoomId*/, const FXmppUserJid& /*UserJid*/);
	/** @return room member exited room delegate */
	virtual FOnXmppRoomMemberExit& OnRoomMemberExit() = 0;

	DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnXmppRoomMemberChanged, const TSharedRef<IXmppConnection>& /*Connection*/, const FXmppRoomId& /*RoomId*/, const FXmppUserJid& /*UserJid*/);
	/** @return room member changed delegate */
	virtual FOnXmppRoomMemberChanged& OnRoomMemberChanged() = 0;

	DECLARE_MULTICAST_DELEGATE_FourParams(FOnXmppRoomChatReceived, const TSharedRef<IXmppConnection>& /*Connection*/, const FXmppRoomId& /*RoomId*/, const FXmppUserJid& /*UserJid*/, const TSharedRef<FXmppChatMessage>& /*ChatMsg*/);
	/** @return chat room message received delegate */
	virtual FOnXmppRoomChatReceived& OnRoomChatReceived() = 0;
};

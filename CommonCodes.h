#ifndef _SmartUppBot2021_CommonCodes_h_
#define _SmartUppBot2021_CommonCodes_h_

#define SEND
#define RECEIVE
#define SEND_RECEIVE
#define CLIENT
#define SERVER
#define CLIENT_SERVER

namespace Upp{
	enum class GatewayOpCodes{
		RECEIVE			DISPATCH = 0,				//An event was dispatched.
		SEND_RECEIVE	HEARTBEAT = 1,				//Fired periodically by the client to keep the connection alive.
		SEND			IDENTIFY = 2,				//Starts a new session during the initial handshake.
		SEND			PRESENCE_UPDATE = 3,		//Update the client's presence.
		SEND			VOICE_STATE_UPDATE = 4,		//Used to join/leave or move between voice channels.
		SEND			RESUME = 6,					//Resume a previous session that was disconnected.
		RECEIVE			RECONNECT= 7,				//You should attempt to reconnect and resume immediately.
		SEND			REQUEST_GUILD_MEMBER = 8,	//Request information about offline guild members in a large guild.
		RECEIVE			INVALID_SESSION = 9,		//The session has been invalidated. You should reconnect and identify/resume accordingly.
		RECEIVE			HELLO = 10,					//Sent immediately after connecting, contains the heartbeat_interval to use.
		RECEIVE			HEARTBEAT_ACK = 11			//Sent in response to receiving a heartbeat to acknowledge that it has been received.
	};
	static String GetGatewayOpCodesMessage(GatewayOpCodes code){
		switch(code){
			case GatewayOpCodes::DISPATCH: return "0 | Dispatch | An event was dispatched"; break;
			case GatewayOpCodes::HEARTBEAT: return "1 | Heartbeat | Fired periodically by the client to keep the connection alive."; break;
			case GatewayOpCodes::IDENTIFY: return "2 | Identify | Starts a new session during the initial handshake."; break;
			case GatewayOpCodes::PRESENCE_UPDATE: return "3 | Presence update | Update the client's presence."; break;
			case GatewayOpCodes::VOICE_STATE_UPDATE: return "4 | Voice state update | Used to join/leave or move between voice channels."; break;
			case GatewayOpCodes::RESUME: return "6 | Resume | Resume a previous session that was disconnected."; break;
			case GatewayOpCodes::RECONNECT: return "7 | Reconnect | You should attempt to reconnect and resume immediately."; break;
			case GatewayOpCodes::REQUEST_GUILD_MEMBER: return "8 | Request Guild Member | Request information about offline guild members in a large guild."; break;
			case GatewayOpCodes::INVALID_SESSION: return "9 | Invalide session | The session has been invalidated. You should reconnect and identify/resume accordingly."; break;
			case GatewayOpCodes::HELLO: return "10 | Hello | Sent immediately after connecting, contains the heartbeat_interval to use."; break;
			case GatewayOpCodes::HEARTBEAT_ACK: return "11 | Heartbeat ack | Sent in response to receiving a heartbeat."; break;
			default: return "Unknow Opcodes"; break;
		};
	}
	
	enum class GatewayCloseEventCodes{
		UNKNOW_ERROR = 4000,			//We're not sure what went wrong. Try reconnecting?
		UNKNOWN_OPCODE = 4001,			//You sent an invalid Gateway opcode or an invalid payload for an opcode. Don't do that!
		DECODE_ERROR = 4002,			//You sent an invalid payload to us. Don't do that!
		NOT_AUTHENTICATED = 4003,		//You sent us a payload prior to identifying.
		AUTHENTICATION_FAILED = 4004,	//The account token sent with your identify payload is incorrect.
		ALREADY_AUTHENTICATED = 4005,	//You sent more than one identify payload. Don't do that!
		INVALID_SEQ = 4007,				//The sequence sent when resuming the session was invalid. Reconnect and start a new session.
		RATE_LIMITED = 4008,			//Woah nelly! You're sending payloads to us too quickly. Slow it down! You will be disconnected on receiving this.
		SESSION_TIMED_OUT = 4009,		//Your session timed out. Reconnect and start a new one.
		INVALID_SHART = 4010,			//You sent us an invalid shard when identifying.
		SHARDING_REQUIRED = 4011,		//The session would have handled too many guilds - you are required to shard your connection in order to connect.
		INVALID_API_VERSION = 4012,		//You sent an invalid version for the gateway.
		INVALID_INTENTS = 4013,			//You sent an invalid intent for a Gateway Intent. You may have incorrectly calculated the bitwise value.
		DISALLOWED_INTENTS = 4014		//You sent a disallowed intent for a Gateway Intent. You may have tried to specify an intent that you have not enabled or are not whitelisted for.
	};
	static String GetGatewayCloseEventCodesMessage(GatewayCloseEventCodes code){
		switch(code){
			case GatewayCloseEventCodes::UNKNOW_ERROR: return "4000 | Unknown Error | We're not sure what went wrong. Try reconnecting?"; break;
			case GatewayCloseEventCodes::UNKNOWN_OPCODE: return "4001 | Unknown op code | You sent an invalid Gateway opcode or an invalid payload for an opcode. Don't do that!"; break;
			case GatewayCloseEventCodes::DECODE_ERROR: return "4002 | Decode error | You sent an invalid payload to us. Don't do that!"; break;
			case GatewayCloseEventCodes::NOT_AUTHENTICATED: return "4003 | Not authenticated | You sent us a payload prior to identifying."; break;
			case GatewayCloseEventCodes::AUTHENTICATION_FAILED: return "4004 | Authentication failled | The account token sent with your identify payload is incorrect."; break;
			case GatewayCloseEventCodes::ALREADY_AUTHENTICATED: return "4005 | Already authenticated | You sent more than one identify payload. Don't do that!"; break;
			case GatewayCloseEventCodes::INVALID_SEQ: return "4007 | Invalide sequence | The sequence sent when resuming the session was invalid. Reconnect and start a new session."; break;
			case GatewayCloseEventCodes::RATE_LIMITED: return "4008 | Rate limited | Woah nelly! You're sending payloads to us too quickly. Slow it down! You will be disconnected on receiving this."; break;
			case GatewayCloseEventCodes::SESSION_TIMED_OUT: return "4009 | Session timed out | Your session timed out. Reconnect and start a new one."; break;
			case GatewayCloseEventCodes::INVALID_SHART: return "4010 | Invalide shart | You sent us an invalid shard when identifying."; break;
			case GatewayCloseEventCodes::SHARDING_REQUIRED: return "4011 | Sharding required | The session would have handled too many guilds - you are required to shard your connection in order to connect."; break;
			case GatewayCloseEventCodes::INVALID_API_VERSION: return "4012 | Invalide api version | You sent an invalid version for the gateway."; break;
			case GatewayCloseEventCodes::INVALID_INTENTS: return "4013 | Invalid intents | You sent an invalid intent for a Gateway Intent. You may have incorrectly calculated the bitwise value."; break;
			case GatewayCloseEventCodes::DISALLOWED_INTENTS: return "4014 | Disallowed intents | You sent a disallowed intent for a Gateway Intent. You may have tried to specify an intent that you have not enabled or are not whitelisted for."; break;
			default: return "Unknow CloseEvent code"; break;
		};
	}
	enum class VoiceOpCodes{
		CLIENT			IDENTIFY = 0,				//Begin a voice websocket connection.
		CLIENT			SELECT_PROTOCOL = 1,		//Select the voice protocol.
		SERVER			READY = 2,					//Complete the websocket handshake.
		CLIENT			HEARTBEAT = 3,				//Keep the websocket connection alive.
		CLIENT			SESSION_DESCRIPTION = 4,	//Describe the session.
		CLIENT_SERVER	SPEAKING = 5,				//Indicate which users are speaking.
		SERVER			HEART_ACK = 6,				//Sent to acknowledge a received client heartbeat.
		CLIENT			RESUME = 7,					//Resume a connection.
		SERVER			HELLO = 8,					//Time to wait between sending heartbeats in milliseconds.
		SERVER			RESUMED = 9,				//Acknowledge a successful session resume.
		SERVER			CLIENT_DISCONNECTED = 13	//A client has disconnected from the voice channel
	};
	static String GetVoiceOpCodesMessage(VoiceOpCodes code){
		switch(code){
			case VoiceOpCodes::IDENTIFY: return "0 | Identify | Begin a voice websocket connection."; break;
			case VoiceOpCodes::SELECT_PROTOCOL: return "1 | Select protocol | Select the voice protocol."; break;
			case VoiceOpCodes::READY: return "2 | Ready | Complete the websocket handshake."; break;
			case VoiceOpCodes::HEARTBEAT: return "3 | Heartbeat | Keep the websocket connection alive."; break;
			case VoiceOpCodes::SESSION_DESCRIPTION: return "4 | Session description | Describe the session."; break;
			case VoiceOpCodes::SPEAKING: return "5 | Speaking | Indicate which users are speaking."; break;
			case VoiceOpCodes::HEART_ACK: return "6 | Heart ack | Sent to acknowledge a received client heartbeat."; break;
			case VoiceOpCodes::RESUME: return "7 | Resume | Resume a connection."; break;
			case VoiceOpCodes::HELLO: return "8 | Hello | Time to wait between sending heartbeats in milliseconds."; break;
			case VoiceOpCodes::RESUMED: return "9 | resumed | Acknowledge a successful session resume."; break;
			case VoiceOpCodes::CLIENT_DISCONNECTED: return "13 | Client disconected | A client has disconnected from the voice channel"; break;
			default: return "Unknow Voice Opcode"; break;
		};
	}
	enum class VoiceCloseEventCodes{
		UNKNOWN_OPCODE = 4001,				//You sent an invalid opcode.
		FAILLED_TO_DECODE_PAYLOAD = 4002,	//You sent a invalid payload in your identifying to the Gateway.
		NOT_AUTHENTICATED = 4003,			//You sent a payload before identifying with the Gateway.
		AUTHENTICATION_FAILED = 4004,		//The token you sent in your identify payload is incorrect.
		ALREADY_AUTHENTICATED = 4005,		//You sent more than one identify payload. Stahp.
		SESSION_NO_LONGER_VALID = 4006,		//Your session is no longer valid.
		SESSION_TIMEOUT = 4009,				//Your session has timed out.
		SERVER_NOT_FOUND = 4011,			//We can't find the server you're trying to connect to.
		UNKNOWN_PROTOCOL = 4012,			//We didn't recognize the protocol you sent.
		DISCONNECTED = 4014,				//Either the channel was deleted or you were kicked. Should not reconnect.
		VOICE_SERVER_CRASHED = 4015,		//The server crashed. Our bad! Try resuming.
		UNKNOWN_ENCRYPTION_MODE = 4016		//We didn't recognize your encryption.
	};
	static String GetVoiceCloseEventMessage(VoiceCloseEventCodes code){
		switch(code){
			case VoiceCloseEventCodes::UNKNOWN_OPCODE: return "4001 | Unknown opcode | You sent an invalid opcode."; break;
			case VoiceCloseEventCodes::FAILLED_TO_DECODE_PAYLOAD: return "4002 | Failled to decode payload | You sent a invalid payload in your identifying to the Gateway."; break;
			case VoiceCloseEventCodes::NOT_AUTHENTICATED: return "4003 | Not authenticated | You sent a payload before identifying with the Gateway."; break;
			case VoiceCloseEventCodes::AUTHENTICATION_FAILED: return "4004 | Authentication failled | The token you sent in your identify payload is incorrect."; break;
			case VoiceCloseEventCodes::ALREADY_AUTHENTICATED: return "4005 | Already authenticated | You sent more than one identify payload. Stahp."; break;
			case VoiceCloseEventCodes::SESSION_NO_LONGER_VALID: return "4006 | Session no longer valid | Your session is no longer valid."; break;
			case VoiceCloseEventCodes::SESSION_TIMEOUT: return "4009 | Session timeout | Your session is no longer valid."; break;
			case VoiceCloseEventCodes::SERVER_NOT_FOUND: return "4011 | Server not found | We can't find the server you're trying to connect to."; break;
			case VoiceCloseEventCodes::UNKNOWN_PROTOCOL: return "4012 | Unknow protocol | We didn't recognize the protocol you sent."; break;
			case VoiceCloseEventCodes::DISCONNECTED: return "4014 | Disconected | Either the channel was deleted or you were kicked. Should not reconnect."; break;
			case VoiceCloseEventCodes::VOICE_SERVER_CRASHED: return "4015 | Voice server crashed | The server crashed. Our bad! Try resuming."; break;
			case VoiceCloseEventCodes::UNKNOWN_ENCRYPTION_MODE: return "4016 | Unknown encryption mode | We didn't recognize your encryption."; break;
			default: return "Unknow CloseEvent code"; break;
		};
	}
	enum class DispatchEvent{
		EVENT_READY = 0,
		EVENT_ERROR = 1,
		GUILD_STATUS = 2,
		GUILD_CREATE = 3,
		CHANNEL_CREATE = 4,
		VOICE_CHANNEL_SELECT = 5,
		VOICE_STATE_UPDATE = 6,
		VOICE_STATE_DELETE = 7,
		VOICE_SETTINGS_UPDATE = 8,
		VOICE_CONNECTION_STATUS = 9,
		SPEAKING_START = 10,
		SPEAKING_STOP = 11,
		MESSAGE_CREATE = 12,
		MESSAGE_UPDATE = 13,
		MESSAGE_DELETE = 14,
		NOTIFICATION_CREATE = 15,
		CAPTURE_SHORTCUT_CHANGE = 16,
		ACTIVITY_JOIN = 17,
		ACTIVITY_SPECTATE = 18,
		ACTIVITY_JOIN_REQUEST = 19,
		MESSAGE_REACTION_ADD = 20,
		UNKNOWN_DISPATCH = 21
	};
	static DispatchEvent GetDispatchEvent(const String& dispatchEvent){
		if(dispatchEvent == "READY"){
			return DispatchEvent::EVENT_READY;
		}else if(dispatchEvent == "ERROR"){
			return DispatchEvent::EVENT_ERROR;
		}else if(dispatchEvent == "GUILD_STATUS"){
			return DispatchEvent::GUILD_STATUS;
		}else if(dispatchEvent == "GUILD_CREATE"){
			return DispatchEvent::GUILD_CREATE;
		}else if(dispatchEvent == "CHANNEL_CREATE"){
			return DispatchEvent::CHANNEL_CREATE;
		}else if(dispatchEvent == "VOICE_CHANNEL_SELECT"){
			return DispatchEvent::VOICE_CHANNEL_SELECT;
		}else if(dispatchEvent == "VOICE_STATE_UPDATE"){
			return DispatchEvent::VOICE_STATE_UPDATE;
		}else if(dispatchEvent == "VOICE_STATE_DELETE"){
			return DispatchEvent::VOICE_STATE_DELETE;
		}else if(dispatchEvent == "VOICE_SETTINGS_UPDATE"){
			return DispatchEvent::VOICE_SETTINGS_UPDATE;
		}else if(dispatchEvent == "VOICE_CONNECTION_STATUS"){
			return DispatchEvent::VOICE_CONNECTION_STATUS;
		}else if(dispatchEvent == "SPEAKING_START"){
			return DispatchEvent::SPEAKING_START;
		}else if(dispatchEvent == "SPEAKING_STOP"){
			return DispatchEvent::SPEAKING_STOP;
		}else if(dispatchEvent == "MESSAGE_CREATE"){
			return DispatchEvent::MESSAGE_CREATE;
		}else if(dispatchEvent == "MESSAGE_UPDATE"){
			return DispatchEvent::MESSAGE_UPDATE;
		}else if(dispatchEvent == "MESSAGE_DELETE"){
			return DispatchEvent::MESSAGE_DELETE;
		}else if(dispatchEvent == "NOTIFICATION_CREATE"){
			return DispatchEvent::NOTIFICATION_CREATE;
		}else if(dispatchEvent == "CAPTURE_SHORTCUT_CHANGE"){
			return DispatchEvent::CAPTURE_SHORTCUT_CHANGE;
		}else if(dispatchEvent == "ACTIVITY_JOIN"){
			return DispatchEvent::ACTIVITY_JOIN;
		}else if(dispatchEvent == "ACTIVITY_SPECTATE"){
			return DispatchEvent::ACTIVITY_SPECTATE;
		}else if(dispatchEvent == "ACTIVITY_JOIN_REQUEST"){
			return DispatchEvent::ACTIVITY_JOIN_REQUEST;
		}else if(dispatchEvent == "MESSAGE_REACTION_ADD"){
			return DispatchEvent::MESSAGE_REACTION_ADD;
		}else{
			return DispatchEvent::UNKNOWN_DISPATCH;
		}
	}
	static String GetDispatchEvent(const DispatchEvent& dispatcheEvent){
		switch(dispatcheEvent){
			case DispatchEvent::EVENT_READY: return "READY"; break;
			case DispatchEvent::EVENT_ERROR: return "ERROR"; break;
			case DispatchEvent::GUILD_STATUS: return "GUILD_STATUS"; break;
			case DispatchEvent::GUILD_CREATE: return "GUILD_CREATE"; break;
			case DispatchEvent::CHANNEL_CREATE: return "CHANNEL_CREATE"; break;
			case DispatchEvent::VOICE_CHANNEL_SELECT: return "VOICE_CHANNEL_SELECT"; break;
			case DispatchEvent::VOICE_STATE_UPDATE: return "VOICE_STATE_UPDATE"; break;
			case DispatchEvent::VOICE_STATE_DELETE: return "VOICE_STATE_DELETE"; break;
			case DispatchEvent::VOICE_SETTINGS_UPDATE: return "VOICE_SETTINGS_UPDATE"; break;
			case DispatchEvent::VOICE_CONNECTION_STATUS: return "VOICE_CONNECTION_STATUS"; break;
			case DispatchEvent::SPEAKING_START: return "SPEAKING_START"; break;
			case DispatchEvent::SPEAKING_STOP: return "SPEAKING_STOP"; break;
			case DispatchEvent::MESSAGE_CREATE: return "MESSAGE_CREATE"; break;
			case DispatchEvent::MESSAGE_UPDATE: return "MESSAGE_UPDATE"; break;
			case DispatchEvent::MESSAGE_DELETE: return "MESSAGE_DELETE"; break;
			case DispatchEvent::NOTIFICATION_CREATE: return "NOTIFICATION_CREATE"; break;
			case DispatchEvent::CAPTURE_SHORTCUT_CHANGE: return "CAPTURE_SHORTCUT_CHANGE"; break;
			case DispatchEvent::ACTIVITY_JOIN: return "ACTIVITY_JOIN"; break;
			case DispatchEvent::ACTIVITY_SPECTATE: return "ACTIVITY_SPECTATE"; break;
			case DispatchEvent::ACTIVITY_JOIN_REQUEST: return "ACTIVITY_JOIN_REQUEST"; break;
			case DispatchEvent::MESSAGE_REACTION_ADD: return "MESSAGE_REACTION_ADD"; break;
			case DispatchEvent::UNKNOWN_DISPATCH: return "UNKNOWN_DIPATCH"; break;
			default: return "UNKNOWN_DIPATCH"; break;
		}
	}
}
#endif

#include "SmartUppBot.h"
#include "AllEmoji.h"
#include "ModuleInfo.h"

#define LLOG(x) LOG(x)

namespace Upp{

SmartUppBot::SmartUppBot(const String& _botID, const String& _botToken) : botToken(_botToken), botID(_botID), lastSequenceNum(0){
	LOG(AsString(GetSysTime()) + " | SmartUppBot is starting...");
	fonctions.Create("help");
	fonctions.Create("uptime");
	fonctions.Create("dumpmembers");
	fonctions.Create("dumproles");
	fonctions.Create("dumpchannels");
	fonctions.Create("dumpemojis");
	fonctions.Create("reload");
	fonctions.Create("delete");
	fonctions.Create("save");
}

SmartUppBot::~SmartUppBot(){
	keepAliveThread.ShutdownThreads();
	keepAliveThread.Wait();
	webSocket.Close();
}

void SmartUppBot::Run(){
	LLOG(AsString(GetSysTime()) + " | Starting SmartUppBot work loop");
	time = GetSysTime();
	for(;;){
		uptime = GetSysTime();
		if(!shouldResume){
			LLOG(AsString(GetSysTime()) + " | No session to resume, Initializing...");
			if(Initializing()){
				LLOG(AsString(GetSysTime()) + " | Connecting...");
				if(Connecting()){
					LLOG(AsString(GetSysTime()) + " | Identifying...");
					if(Identifying()){
						InitThread(sessionID,heartbeatInterval);
						Listen();
						CloseThread();
					}
				}
			}
		}else{
			LLOG(AsString(GetSysTime()) + " | Starting remusing process...");
			if(Connecting(false)){
				LLOG(AsString(GetSysTime()) + " | Identifying...");
				if(Identifying()){
					LLOG(AsString(GetSysTime()) + " | Resuming...");
					if(Resuming()){
						InitThread(sessionID,heartbeatInterval);
						Listen();
						CloseThread();
					}
				}
			}else{
				shouldResume = false;
			}
		}
		if(shouldResume){
			LOG(AsString(GetSysTime()) + " | Something went wrong... SmartUppBot go for resuming the connection...");
		}else{
			LOG(AsString(GetSysTime()) + " | Something went wrong... Waiting for " + AsString(TIME_IN_SECOND()) + " seconds before reconection...");
			Sleep(TIME_BETWEEN_CONNEXION);
		}
	}
}

void SmartUppBot::Listen(){
	LOG(AsString(GetSysTime()) + " | SmartUppBot is now listening");
	for(;;){
		String response = webSocket.Receive();
		if(webSocket.IsError()) {
			LOG(AsString(GetSysTime()) + " | WebSocket error : " + webSocket.GetError());
			shouldResume = false;
			break;
        }else if(webSocket.IsClosed()){
            LOG(AsString(GetSysTime()) + " | WebSocket has been close");
			shouldResume = false;
			break;
        }
		
		ValueMap payload = ParseJSON(response);
        int op = payload["op"];
		
		if(op != (int)GatewayOpCodes::HEARTBEAT_ACK) LOG(AsString(GetSysTime()) + " | " + GetGatewayOpCodesMessage((Upp::GatewayOpCodes)op));
		if(op == (int)GatewayOpCodes::DISPATCH){
			Dispatch(payload);
		}else if(op == (int)GatewayOpCodes::INVALID_SESSION){
			LOG(AsString(GetSysTime()) + " | Invalid session, gateway unable to resume!");
			shouldResume = false;
			break;
		}else if(op == (int)GatewayOpCodes::HEARTBEAT_ACK){
			WaitingForHeartBeatACK = false;
		}else if(op == (int)GatewayOpCodes::RECONNECT){
			LLOG(AsString(GetSysTime()) + " | Invalid session, Reconnection is needed !");
			shouldResume = true;
			break;
		}
	}
}

void SmartUppBot::InitThread(unsigned int session_id, unsigned int heartbeat_interval ){ //Init the connexion by setting Heartbeat thread and then call Identify procedure
	LLOG(AsString(GetSysTime()) + " | Initialising Keep alive thread...");
	CloseThread();
	keepAliveStart = true;
	WaitingForHeartBeatACK = false;
	keepAliveThread.Run(THISBACK1(KeepAlive,heartbeatInterval));
	LLOG(AsString(GetSysTime()) + " | Keep alive run launched !");
}

void SmartUppBot::CloseThread(){
	LLOG(AsString(GetSysTime()) + " | Closing Keep alive thread...");
	if(keepAliveThread.IsOpen()){
		keepAliveStart = false;
		//keepAliveThread.S;
		keepAliveThread.Wait();
		LLOG(AsString(GetSysTime()) + " | Keep alive is now closed !");
	}else{
		LLOG(AsString(GetSysTime()) + " | No thread to be closed...");
	}
}

void SmartUppBot::InitGuild(ValueMap& payload ,const String& guildID){
	LOG(AsString(GetSysTime()) + " | Trying to load the guild object...");
	guild.Clear();
	
	discordRequest.New();
    String guildJson = discordRequest.Url(baseUrl)
           .Path("/api/v8/guilds/" + guildID)
           .GET()
           .Execute();
           
    ApplyRateLimits(discordRequest);
    discordRequest.Close();
    
    discordRequest.New();
    String channelJson = discordRequest.Url(baseUrl)
           .Path("/api/v8/guilds/" + guildID +"/channels")
           .GET()
           .Execute();
           
    ApplyRateLimits(discordRequest);
    discordRequest.Close();

	discordRequest.New();
	bool noMembers = false;
	int previousCount = 0;
	int maxGuildMember =(int) stou64(~(payload["d"]["member_count"]));
    String membersJson = discordRequest.Url(baseUrl)
           .Path("/api/v8/guilds/" + guildID +"/members?limit=" + AsString(((maxGuildMember < 1000 )? maxGuildMember : 1000)))
           .GET()
           .Execute();
           
    if(discordRequest.IsHttpError()){
		LOG(AsString(GetSysTime()) + " | Impossible to retrieve members of guild, check for good intent in Discord developper portal !");
		noMembers = true;
    }
    ApplyRateLimits(discordRequest);
    discordRequest.Close();
    previousCount = ((maxGuildMember < 1000 )? maxGuildMember : 1000);
	maxGuildMember -= ((maxGuildMember < 1000 )? maxGuildMember : 1000);
	
    guild.Build(ParseJSON(guildJson),ParseJSON(channelJson),ParseJSON(membersJson));
    if(!noMembers){
	    while(maxGuildMember > 0){
			discordRequest.New();
		    membersJson = discordRequest.Url(baseUrl)
		           .Path("/api/v8/guilds/" + guildID +"/members?limit=" + AsString(((maxGuildMember < 1000 )? maxGuildMember : 1000)) +"&after=" + AsString(previousCount))
		           .GET()
		           .Execute();
		           
		    ApplyRateLimits(discordRequest);
		    discordRequest.Close();
		    previousCount += ((maxGuildMember < 1000 )? maxGuildMember : 1000);
			maxGuildMember -= ((maxGuildMember < 1000 )? maxGuildMember : 1000);
			guild.AddMembers(ParseJSON(membersJson));
	    }
    }
    if(guild.IsLoaded()) LOG(AsString(GetSysTime()) + " | Guild object have been loaded successfully !");
    else LOG(AsString(GetSysTime()) + " | Error during guild object loading...");
}

void SmartUppBot::Dispatch(ValueMap& payload){
	try{
		Vector<Command> allCmds;
		String dispatchEvent = payload["t"];
        lastSequenceNum = (int)payload["s"];
        DispatchEvent dispatch = GetDispatchEvent(dispatchEvent);
        switch(dispatch){
			case DispatchEvent::EVENT_READY:
				LOG(AsString(GetSysTime()) + " | Dispatch event READY");
			break;
			case DispatchEvent::EVENT_ERROR:
				LOG(AsString(GetSysTime()) + " | Dispatch event ERROR");
			break;
			case DispatchEvent::GUILD_STATUS:
				LOG(AsString(GetSysTime()) + " | Dispatch event GUILD_STATUS");
			break;
			case DispatchEvent::GUILD_CREATE:
				InitGuild(payload, ~payload["d"]["id"]);
				LoadConfig();
				
				LOG(AsString(GetSysTime()) + " | Dispatch event GUILD_CREATE");
			break;
			case DispatchEvent::CHANNEL_CREATE:
				LOG(AsString(GetSysTime()) + " | Dispatch event CHANNEL_CREATE");
			break;
			case DispatchEvent::VOICE_CHANNEL_SELECT:
				LOG(AsString(GetSysTime()) + " | Dispatch event VOICE_CHANNEL_SELECT");
			break;
			case DispatchEvent::VOICE_STATE_UPDATE:
				LOG(AsString(GetSysTime()) + " | Dispatch event VOICE_STATE_UPDATE");
			break;
			case DispatchEvent::VOICE_STATE_DELETE:
				LOG(AsString(GetSysTime()) + " | Dispatch event VOICE_STATE_DELETE");
			break;
			case DispatchEvent::VOICE_SETTINGS_UPDATE:
				LOG(AsString(GetSysTime()) + " | Dispatch event VOICE_SETTINGS_UPDATE");
			break;
			case DispatchEvent::VOICE_CONNECTION_STATUS:
				LOG(AsString(GetSysTime()) + " | Dispatch event VOICE_CONNECTION_STATUS");
			break;
			case DispatchEvent::SPEAKING_START:
				LOG(AsString(GetSysTime()) + " | Dispatch event SPEAKING_START");
			break;
			case DispatchEvent::SPEAKING_STOP:
				LOG(AsString(GetSysTime()) + " | Dispatch event SPEAKING_STOP");
			break;
			case DispatchEvent::MESSAGE_CREATE:
				LOG(AsString(GetSysTime()) + " | Dispatch event MESSAGE_CREATE");
				allCmds = ParseCommand(~payload["d"]["content"]);
				for(Command& cmd : allCmds){
					DispatchLog trouver = DispatchLog::NO_FUNCTION;
					if(cmd.prefix.GetCount() == 0 && cmd.functionName.IsEqual("help") && (trouver = CheckRight("help", payload)) == DispatchLog::DONE ){
						Help(payload,cmd);
					}else if(cmd.prefix.GetCount() == 0 && cmd.functionName.IsEqual("uptime") && (trouver = CheckRight("uptime", payload)) == DispatchLog::DONE){
						Uptime(payload,cmd);
					}else if(cmd.prefix.GetCount() == 0 && cmd.functionName.IsEqual("delete") && (trouver = CheckRight("delete", payload)) == DispatchLog::DONE){
						Delete(payload,cmd);
					}else if(cmd.prefix.GetCount() == 0 && cmd.functionName.IsEqual("dumpmembers") && (trouver = CheckRight("dumpmembers", payload)) == DispatchLog::DONE){
						DumpMembers(payload,cmd);
					}else if(cmd.prefix.GetCount() == 0 && cmd.functionName.IsEqual("dumproles") && (trouver = CheckRight("dumproles", payload)) == DispatchLog::DONE){
						DumpRoles(payload,cmd);
					}else if(cmd.prefix.GetCount() == 0 && cmd.functionName.IsEqual("dumpchannels") && (trouver = CheckRight("dumpchannels", payload)) == DispatchLog::DONE){
						DumpChannels(payload,cmd);
					}else if(cmd.prefix.GetCount() == 0 && cmd.functionName.IsEqual("dumpemojis") && (trouver = CheckRight("dumpemojis", payload)) == DispatchLog::DONE){
						DumpEmojis(payload,cmd);
					}else if(cmd.prefix.GetCount() == 0 && cmd.functionName.IsEqual("save") && (trouver = CheckRight("save", payload)) == DispatchLog::DONE){
						SaveConfig();
					}else if(cmd.prefix.GetCount() == 0 && cmd.functionName.IsEqual("reload") && (trouver = CheckRight("reload", payload)) == DispatchLog::DONE){
							LoadConfig();
					}else{
						for(Module& mod : modules){
							if(mod.TestPrefixes(cmd.prefix))
								trouver = mod.DispatchCommand(payload,cmd);
						}
					}
					if(trouver == DispatchLog::NO_FUNCTION){
						LOG(AsString(GetSysTime()) + " | function named " + cmd.functionName + ((cmd.prefix.GetCount() >0)? " with prefix " + cmd.prefix :"") +" is incorrect");
						CreateMessage(payload["d"]["channel_id"], "function named " + cmd.functionName + ((cmd.prefix.GetCount() >0)? " with prefix " + cmd.prefix :"") + " is incorrect");
					}else if(trouver == DispatchLog::NOT_ALLOWED){
						LOG(AsString(GetSysTime()) + " | function named " + cmd.functionName + ((cmd.prefix.GetCount() >0)? " with prefix " + cmd.prefix :"") +" is not allowed for " + ~payload["d"]["author"]["username"]);
						CreateMessage(payload["d"]["channel_id"], "function named " + cmd.functionName + ((cmd.prefix.GetCount() >0)? " with prefix " + cmd.prefix :"") +" is not allowed for " + ~payload["d"]["author"]["username"]);
					}
				}
				/*
	            if( (~payload["d"]["author"]["username"]).IsEqual("Xemuth")){
					ValueMap data = CreateReaction(payload["d"]["channel_id"], payload["d"]["id"], "heart");
				}else if((~payload["d"]["author"]["id"]).IsEqual("131865910121201664")){
					ValueMap data = CreateReaction(payload["d"]["channel_id"], payload["d"]["id"], "nodrug");
				}
				*/
			break;
			case DispatchEvent::MESSAGE_UPDATE:
				LOG(AsString(GetSysTime()) + " | Dispatch event MESSAGE_UPDATE");
			break;
			case DispatchEvent::MESSAGE_DELETE:
				LOG(AsString(GetSysTime()) + " | Dispatch event MESSAGE_DELETE");
			break;
			case DispatchEvent::NOTIFICATION_CREATE:
				LOG(AsString(GetSysTime()) + " | Dispatch event NOTIFICATION_CREATE");
			break;
			case DispatchEvent::CAPTURE_SHORTCUT_CHANGE:
				LOG(AsString(GetSysTime()) + " | Dispatch event CAPTURE_SHORTCUT_CHANGE");
			break;
			case DispatchEvent::ACTIVITY_JOIN:
				LOG(AsString(GetSysTime()) + " | Dispatch event ACTIVITY_JOIN");
			break;
			case DispatchEvent::ACTIVITY_SPECTATE:
				LOG(AsString(GetSysTime()) + " | Dispatch event ACTIVITY_SPECTATE");
			break;
			case DispatchEvent::ACTIVITY_JOIN_REQUEST:
				LOG(AsString(GetSysTime()) + " | Dispatch event ACTIVITY_JOIN_REQUEST");
			break;
			case DispatchEvent::MESSAGE_REACTION_ADD:
				LOG(AsString(GetSysTime()) + " | Dispatch event ACTIVITY_JOIN_REQUEST");
			break;
			case DispatchEvent::UNKNOWN_DISPATCH:
				LOG(AsString(GetSysTime()) + " | Unknown event named " + dispatchEvent);
			break;
			default:
				LOG(AsString(GetSysTime()) + " | Unknown event named " + dispatchEvent);
			break;
		}
	}catch(Exc& exception){
		LOG(AsString(GetSysTime()) + " | " + exception);
	}
}

DispatchLog SmartUppBot::CheckRight(const String& str, ValueMap& payload){
	for(Fonction& fonc : fonctions){
		if(ToLower(fonc.command).IsEqual(ToLower(str))){
			Value userRoles = payload["d"]["member"]["roles"];
			String author = payload["d"]["author"]["id"];
			for(const String& str : fonc.usersAllowed){
				if(author.IsEqual(str)){
					LOG(AsString(GetSysTime()) + " | Dispatching fonction " + str);
					return DispatchLog::DONE;
				}
			}
			if(fonc.rolesAllowed.GetCount() == 0 && fonc.usersAllowed.GetCount() == 0){
				LOG(AsString(GetSysTime()) + " | Dispatching fonction " + str);
				return DispatchLog::DONE;
			}else{
				for(int i = 0; i < userRoles.GetCount(); i++){
					for(const String& str : fonc.rolesAllowed){
						if(userRoles[i].Get<String>().IsEqual(str)){
							LOG(AsString(GetSysTime()) + " | Dispatching fonction " + str);
							return DispatchLog::DONE;
						}
					}
				}
				return DispatchLog::NOT_ALLOWED;
			}
		}
	}
	return DispatchLog::NO_FUNCTION;
}

bool SmartUppBot::Initializing(){
	discordRequest.Clear();
	discordRequest.Header("User-Agent", botID)
	   .Header("Authorization", "Bot " + botToken)
	   .ContentType("application/json");
	   
	discordRequest.New();
    String response = discordRequest.Url(baseUrl + "/api/gateway/bot").Execute();
    if(response.GetCount()){
		gatewayAddr = ParseJSON(response)["url"];
		LOG(AsString(GetSysTime()) + " | Initializing done !");
		return true;
    }else{
		LOG(AsString(GetSysTime()) + " | Error Initializing. The discord gateway is impossible to reach : " + discordRequest.GetErrorDesc());
		return false;
	}
}

bool SmartUppBot::Connecting(bool reset){
	LOG(AsString(GetSysTime()) + " | Connecting to the discord gateway for bot ID No " + AsString(botID) +"...");
	if(webSocket.IsOpen()){
		webSocket.Close();
		LLOG(AsString(GetSysTime()) + " | Websocket closed...");
	}
	if(reset){
		LLOG(AsString(GetSysTime()) + " | Preparing WebSocket...");
		(&webSocket)->~WebSocket();
		new (&webSocket) WebSocket();
	    webSocket.Headers("Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n"
						  "Accept-Language: cs,en-US;q=0.7,en;q=0.3\r\n"
						  "Sec-WebSocket-Version: 13\r\n"
						  "Sec-WebSocket-Extensions: permessage-deflate\r\n"
						  "Connection: keep-alive, Upgrade\r\n"
						  "Pragma: no-cache\r\n"
						  "Cache-Control: no-cache\r\n"
						  "Upgrade: websocket\r\n");
						  
		webSocket.Header("User-Agent", botID)
	      .Header("Authorization", "Bot " + botToken)
	      .Header("ContentType", "application/json");
	}
	LLOG(AsString(GetSysTime()) + " | Websocket connect...");
	webSocket.Connect(gatewayAddr, "gateway.discord.gg", true, 443);
	
	if(webSocket.IsError()){
		LOG(AsString(GetSysTime()) + " | Error connecting to the discord gateway : " + webSocket.GetErrorDesc());
		return false;
	}else if(webSocket.IsClosed()){
		LOG(AsString(GetSysTime()) + " | Connecting can't be done. Websocket is close : " + webSocket.GetErrorDesc());
		return false;
    }else{
        LLOG(AsString(GetSysTime()) + " | Connection succeed ! Receving Hello message...");
		ValueMap payload = ParseJSON(webSocket.Receive());
        int op = payload["op"];
        if((GatewayOpCodes)op == GatewayOpCodes::HELLO){
            LOG(AsString(GetSysTime()) + " | Hello event have been received, it's time to indentifying" );
            sessionID = payload["d"]["session_id"].Get<int>();
			heartbeatInterval = (int) payload["d"]["heartbeat_interval"];
			LOG(AsString(GetSysTime()) + " | Session ID : " + AsString(sessionID));
			LOG(AsString(GetSysTime()) + " | HeartbeatInterval : " + AsString(heartbeatInterval) + " ms");
            return true;
        }else{
            LOG(AsString(GetSysTime()) + " | Error connecting to the discord gateway : The gateway have not respond to our connection by Hello event : " + payload.ToString());
			return false;
        }
	}
}

bool SmartUppBot::Identifying(){
	LOG(AsString(GetSysTime()) + " | Starting Identifying...");
	Json json;
    json("op", int(GatewayOpCodes::IDENTIFY))
		("d", Json("token", botToken)
				  ("properties",Json("$os", "Windows")
									("$browser", "disco")
									("$device", "disco"))
				  ("compress", false)
				  ("large_threshold", 250)
				  ("presence",Json("game",Json("name", "Ultimate++")
											  ("type", 0))
				  ("status", "online")
				  ("since", int(Null))
				  ("afk", false)));

    if(webSocket.IsError()){
        LOG(AsString(GetSysTime()) + " | Identification can't be done. Websocket is on error : " + webSocket.GetErrorDesc());
		return false;
    }else if(webSocket.IsClosed()){
		LOG(AsString(GetSysTime()) + " | Identification can't be done. Websocket is close, make sure you called Connecting before identifying fonctions !");
		return false;
    }else{
        webSocket.SendTextMasked(json);
        if(webSocket.IsError()){
			LOG(AsString(GetSysTime()) + " | Identification have failled..." + webSocket.GetErrorDesc());
			return false;
	    }else if(webSocket.IsClosed()){
			LOG(AsString(GetSysTime()) + " | Identification have failled. Websocket is close, make sure you called Connecting before identifying fonctions !");
			return false;
        }else{
			LOG(AsString(GetSysTime()) + " | Identification succed ! the keep alive thread can now be launch !");
			return true;
	    }
    }
}

bool SmartUppBot::Resuming(){
	Json resuming;
    resuming("op", int(GatewayOpCodes::RESUME))
	        ("d", Json("token", botToken)
				 ("session_id", (int)sessionID)
				 ("seq",(int)lastSequenceNum));
				 
    if(webSocket.IsError()){
        LOG(AsString(GetSysTime()) + " | Resuming can't be done. Websocket is on error : " + webSocket.GetErrorDesc());
		return false;
    }else if(webSocket.IsClosed()){
		LOG(AsString(GetSysTime()) + " | Resuming can't be done. Websocket is close, make sure you called Connecting before resuming fonctions !");
		return false;
    }else{
        webSocket.SendTextMasked(resuming.ToString());
        ValueMap payload = ParseJSON(webSocket.Receive());
        int op = payload["op"];
        if(webSocket.IsError()){
			LOG(AsString(GetSysTime()) + " | Resuming have failled..." + webSocket.GetErrorDesc());
			return false;
	    }else{
			LOG(AsString(GetSysTime()) + " | Resuming succed ! the keep alive thread can now be launch !");
			return true;
	    }
    }
}

void SmartUppBot::LoadConfig(){
	if(FileExists(GetExeFolder() + "/" + GetExeTitle()  +".cfg")){
		FileIn in(GetExeFolder() + "/" + GetExeTitle()  +".cfg");
		if(in){
			String file = in.Get(999999);
			Value vm = ParseJSON(file);
			in.Close();
			bool trouver = false;
			for(int e = 0; e < vm["functions"].GetCount(); e++){
				trouver = false;
				for(Module& m : modules){
					for(Fonction& fonc : m.GetHandlers()){
						if( ToLower(fonc.command).IsEqual(ToLower((~vm["functions"][e]["name"])))){
							fonc.rolesAllowed.Clear();
							fonc.usersAllowed.Clear();
							FillRolesAllowed(fonc, e, vm);
							FillUsersAllowed(fonc, e, vm);
							trouver = true;
							break;
						}
					}
					if(trouver) break;
				}
				if(!trouver){
					//We look for default fonction of bot
					for(Fonction& fonc : fonctions){
						if( ToLower(fonc.command).IsEqual(ToLower((~vm["functions"][e]["name"])))){
							FillRolesAllowed(fonc, e, vm);
							FillUsersAllowed(fonc, e, vm);
							break;
						}
					}
				}
			}
		}
	}
}

void SmartUppBot::FillRolesAllowed(Fonction& fonc, int foncCount, Value& vm){
	for(int i = 0; i < vm["functions"][foncCount]["roles"].GetCount(); i++){
		if(vm["functions"][foncCount]["roles"][i]["ID"].IsNull()){
			String newID = GetIDFromRoleName(vm["functions"][foncCount]["roles"][i]["name"]);
			if(newID.GetCount())
				fonc.rolesAllowed.Add(newID);
			else
				LOG(AsString(GetSysTime()) + " | Can't solve role name " + ~vm["functions"][foncCount]["roles"][i]["name"]);
		}else{
			fonc.rolesAllowed.Add((~vm["functions"][foncCount]["roles"][i]["ID"]));
		}
	}
}

void SmartUppBot::FillUsersAllowed(Fonction& fonc, int foncCount, Value& vm){
	for(int i = 0; i < vm["functions"][foncCount]["users"].GetCount(); i++){
		if(vm["functions"][foncCount]["users"][i]["ID"].IsNull()){
			String newID = GetIDFromUserName(vm["functions"][foncCount]["users"][i]["name"]);
			if(newID.GetCount())
				fonc.usersAllowed.Add(newID);
			else
				LOG(AsString(GetSysTime()) + " | Can't solve user name " + ~vm["functions"][foncCount]["users"][i]["name"]);
		}else{
			fonc.usersAllowed.Add((~vm["functions"][foncCount]["users"][i]["ID"]));
		}
	}
}

void SmartUppBot::SaveConfig(){
	FileOut out(GetExeFolder() + "/" + GetExeTitle()  +".cfg");
	if(out){
		out << "{\n";
		out <<"\t\"_comment\": \"Functions is an array of each function definig is allowed roles / users. each roles/users can be writted in Str or ID\",\n";
		out <<"\t\"_comment\": \"the config file must be at the same place of the bot and must be named like the bot exe but with .cfg\",\n";
		out <<"\t\"_comment\": \"ID is prevalent over the rest, if users/roles can't be resolved when guild is loaded then it will be deleted and wont be saved\",\n";
		out <<"\t\"_comment\": \"if no ID is present, users will try to be resolved then ID will be added\",\n";
		out <<"\t\"functions\":[\n";
		for(Module& mod : modules){
			for(int i = 0; i < mod.GetHandlers().GetKeys().GetCount(); i++){
				const String& fnStr = mod.GetHandlers().GetKeys()[i];
				Fonction& fn = mod.GetHandlers().Get(fnStr);
				out << "\t\t{\n\t\t\t\"name\":\""+ fn.command +"\",\n\t\t\t\"roles\":[";
				for(int e = 0; e < fn.rolesAllowed.GetCount(); e++){
					String& str = fn.rolesAllowed[e];
					out << "{\"name\":\"" + GetNameByRoleID(str) + "\", \"ID\":\"" + str +"\"}" << ((e < fn.rolesAllowed.GetCount() -1)?",":"");
				}
				out << "],\n\t\t\t";
				out << "\"users\":[";
				for(int e = 0; e < fn.usersAllowed.GetCount(); e++){
					String& str = fn.usersAllowed[e];
					out << "{\"name\":\"" + GetNameByUserID(str) + "\", \"ID\":\"" + str +"\"}" << ((e < fn.usersAllowed.GetCount() -1)?",":"");
				}
				out << "]\n\t\t}" << ((i < mod.GetHandlers().GetKeys().GetCount() -1)?",":"");
			}
		}
		out <<"\n\t]\n}";
		out.Close();
	}
}

String SmartUppBot::GetNameByRoleID(const String& str){
	if(str.GetCount()){
		for(const Role& role : guild.GetRoles()){
			if(ToLower(role.GetID()).IsEqual(ToLower(str))){
				return role.GetName();
			}
		}
	}
	return "";
}
String SmartUppBot::GetNameByUserID(const String& str){
	if(str.GetCount()){
		for(const Member& member : guild.GetMembers()){
			if(ToLower(member.GetID()).IsEqual(ToLower(str))){
				return member.GetUsername();
			}
		}
	}
	return "";
}

String SmartUppBot::GetIDFromRoleName(const String& str){
	if(str.GetCount()){
		for(const Role& role : guild.GetRoles()){
			if(ToLower(role.GetName()).IsEqual(ToLower(str))){
				return role.GetID();
			}
		}
	}
	return "";
}
String SmartUppBot::GetIDFromUserName(const String& str){
	if(str.GetCount()){
		for(const Member& member : guild.GetMembers()){
			if(ToLower(member.GetUsername()).IsEqual(ToLower(str))){
				return member.GetID();
			}
		}
	}
	return "";
}

void SmartUppBot::KeepAlive(unsigned int time){
	LOG(AsString(GetSysTime()) + " | Starting keep alive thread");
	int divided = time/1000;
	bool exitThread = false;
	
    while(!Thread::IsShutdownThreads()) {
        for(int i = 0; i < divided; i++){
            if(Thread::IsShutdownThreads()) exitThread = true;
            if(keepAliveStart == false) exitThread = true;
            if(exitThread)break;
            Thread::Sleep(1000);
        }
        if(exitThread) break;
		if(!WaitingForHeartBeatACK){
		
	        Json ack;
	        ack("op", int(GatewayOpCodes::HEARTBEAT))
	           ("d",  (int)lastSequenceNum);
	           
	        //LOG(AsString(GetSysTime()) + " | Sending heartbeat : " + ack.ToString());
	        if(webSocket.IsOpen()){
				webSocket.SendTextMasked(ack.ToString());
				WaitingForHeartBeatACK = true;
	        }else{
				LOG(AsString(GetSysTime()) + " | Can't send heart bit. Socket is close");
	        }
		}else{
			LOG(AsString(GetSysTime()) + " | Heartbeat ACK have not been received ! Keep alive thread close the socket and terminate itself");
			webSocket.Close();
			break;
		}
    }
    LOG(AsString(GetSysTime()) + " | Stopping keep alive thread");
}

void SmartUppBot::ApplyRateLimits(HttpRequest& req) {
	//Taken from Discord package of jjacksonRIAB
    int Limit     = StrInt(req.GetHeader("x-ratelimit-limit"));
    int Remaining = StrInt(req.GetHeader("x-ratelimit-remaining"));
    int WaitUntil = StrInt(req.GetHeader("x-ratelimit-reset"));
    
    Time wait = TimeFromUTC(WaitUntil);
    auto waitSeconds = GetUTCSeconds(wait) + GetLeapSeconds(wait) + GetTimeZone() * 60;
    
    Time date;
    ScanWwwTime(req.GetHeader("date"), date);
    auto serverSeconds = GetUTCSeconds(date);
    
    auto TimeDiff = waitSeconds - serverSeconds;
    
    if(Remaining == 0) {
        Thread::Sleep((int)TimeDiff * 1000);
    }
}

DispatchLog Module::DispatchCommand(ValueMap& payload, Command& dispatched){
	if(handlers.Find(dispatched.functionName) != -1){
		Fonction& fn = handlers.Get(dispatched.functionName);
		Value userRoles = payload["d"]["member"]["roles"];
		String author = payload["d"]["author"]["id"];
		for(const String& str : fn.usersAllowed){
			if(author.IsEqual(str)){
				LOG(AsString(GetSysTime()) + " | Dispatching fonction " + dispatched.functionName +" in module named " + GetName() +" with arguments : " + dispatched.values.ToString());
				fn.event(payload,dispatched);
				return DispatchLog::DONE;
			}
		}
		
		if(fn.rolesAllowed.GetCount() == 0 && fn.usersAllowed.GetCount() == 0){
			LOG(AsString(GetSysTime()) + " | Dispatching fonction " + dispatched.functionName +" in module named " + GetName() +" with arguments : " + dispatched.values.ToString());
			fn.event(payload,dispatched);
			return DispatchLog::DONE;
		}else{
			for(int i = 0; i < userRoles.GetCount(); i++){
				for(const String& str : fn.rolesAllowed){
					if(userRoles[i].Get<String>().IsEqual(str)){
						LOG(AsString(GetSysTime()) + " | Dispatching fonction " + dispatched.functionName +" in module named " + GetName() +" with arguments : " + dispatched.values.ToString());
						fn.event(payload,dispatched);
						return DispatchLog::DONE;
					}
				}
			}
			return DispatchLog::NOT_ALLOWED;
		}
	}
	return DispatchLog::NO_FUNCTION;
}


String DisplayEllaspedtime(const Time& t){
	String str;
	int year = (int)t.year - 1970;
	int month = (int)t.month - 1;
	int day = (int)t.day - 1;
	int hour = (int)t.hour;
	int minute =(int)t.minute;
	int second =(int)t.second;
	if(year > 0){
		str << AsString(year) << " year" << ((year > 1)?"s":"") << " ";
	}
	if(month > 0){
		str << AsString(month) << " month" << ((month > 1)?"s":"") << " ";
	}
	if(day > 0){
		str << AsString(day) << " day" << ((day > 1)?"s":"") << " ";
	}
	if(hour > 0){
		str << AsString(hour) << " hour" << ((hour > 1)?"s":"") << " ";
	}
	if(minute > 0){
		str << AsString(minute) << " minute" << ((minute > 1)?"s":"") << " ";
	}
	str << AsString(second) << " second" << ((second > 1)?"s":"") << " ";
	return str;
}

bool IsANumber(const Upp::String& str){
	if (std::isdigit(str[0]) || (str.GetCount() > 1 && (str[0] == '+'))){
        for (int i = 1 ; i < str.GetCount(); ++i)
            if (!std::isdigit(str[i]))
                return false;
        return true;
    }
    return false;
}

Vector<Command> ParseCommand(const String& command){
	Vector<Command> commands;
	Vector<String> allCmds = Split(command,";",true);
	
	for(const String& str : allCmds){
		if(str.StartsWith("!")){
			Command& bufferCmd = commands.Add();
			String strCopy = str.Mid(1,str.GetCount() -1);
			strCopy = TrimBoth(strCopy);
			int parathesePosition = strCopy.Find("(",1);
			if(parathesePosition != -1){
				int endParathesePosition = strCopy.Find(")",1);
				if(endParathesePosition == -1) endParathesePosition = strCopy.GetCount();
				String Arguments = strCopy.Mid(parathesePosition+1, endParathesePosition - (parathesePosition+1));
				Arguments = TrimBoth(Arguments);
				strCopy = strCopy.Left(parathesePosition);
				
				while(Arguments.GetCount()){
					int segment = -1;
					
					Arguments = TrimBoth(Arguments);
					if(Arguments.StartsWith(","))
						Arguments = Arguments.Mid(1,Arguments.GetCount() -1);
					Arguments = TrimBoth(Arguments);
					
					if(Arguments.StartsWith("\"")){
						Arguments = Arguments.Mid(1,Arguments.GetCount() -1);
						segment = Arguments.Find("\"");
					}
					if(segment == -1){
						segment = Arguments.Find(",", 1);
						if(segment > 0)segment -= 1;
					}
					if(segment == -1){
						segment = Arguments.GetCount();
					}
					String freshArg = TrimBoth(Arguments.Left(segment));
					if(IsANumber(freshArg))
						bufferCmd.values.Create((double)stou64(freshArg));
					else
						bufferCmd.values.Create(freshArg);
					
					Arguments = Arguments.Mid(segment + 1, Arguments.GetCount() - segment);
				}
			}
			int espacePosition = strCopy.Find(" ");
			if( espacePosition != -1){
				bufferCmd.prefix = ToLower(strCopy.Left(espacePosition));
				strCopy = strCopy.Mid(espacePosition,strCopy.GetCount() - espacePosition);
			}
			bufferCmd.functionName = ToLower(TrimBoth(strCopy));
		}
	}
	return commands;
}

unsigned int Levensthein_Distance(const String& s1, const String& s2){
	int len1 = s1.GetCount(), len2 = s2.GetCount();
	Vector<Vector<unsigned int>> d;
	d.AddN(len1 + 1);
	for(Vector<unsigned int> & myVector :d){
		myVector.AddN(len2 + 1);
	}
	d[0][0] = 0;
	for(int i = 1; i <= len1; ++i) d[i][0] = i;
	for(int i = 1; i <= len2; ++i) d[0][i] = i;
	for(int i = 1; i <= len1; ++i)
		for(int j = 1; j <= len2; ++j)
                      // note that std::min({arg1, arg2, arg3}) works only in C++11,
                      // for C++98 use std::min(std::min(arg1, arg2), arg3)
                      d[i][j] = std::min({ d[i - 1][j] + 1, d[i][j - 1] + 1, d[i - 1][j - 1] + (s1[i - 1] == s2[j - 1] ? 0 : 1) });
	return d[len1][len2];
}

};



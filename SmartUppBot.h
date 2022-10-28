#ifndef _SmartUppBot2021_SmartUppBot_h_
#define _SmartUppBot2021_SmartUppBot_h_

#include <Core/Core.h>
#include "CommonCodes.h"

#include "DiscordServerArchitecture.h"

#define TIME_BETWEEN_CONNEXION 30000
#define TIME_IN_SECOND() (TIME_BETWEEN_CONNEXION/1000)

#define REGISTER(FonctionName) RegisterFonction([&](ValueMap& payload, Command& dispatched){FonctionName(payload,dispatched);}, #FonctionName);
#define REGISTER_PREFIX(PrefixName) RegisterPrefix(PrefixName);
#define REGISTER_NO_PREFIX()  RegisterPrefix("");

static const Upp::String version = "SmartUppBot -- V0.3 -- 08/02/2021";

namespace Upp{
	class SmartUppBot;
	
	String DisplayEllaspedtime(const Time& t);
	bool IsANumber(const String& str);
	unsigned int Levensthein_Distance(const String& s1, const String& s2);
	
	
	struct Command : public Moveable<Command>{
		String prefix;
		String functionName;
		Vector<Value> values;
		String ToString()const{	return "prefix: " + prefix + "\nfunctionName: " + functionName +"\nvalues: "+ values.ToString();}
	};
	Vector<Command> ParseCommand(const String& command);
	
	struct Fonction  : public Moveable<Fonction>{
		Fonction(){}
		Fonction(const String& _command) : command(_command){}
		String command;
		Vector<String> rolesAllowed;
		Vector<String> usersAllowed;
		Event<ValueMap&, Command&> event;
	};
	
	enum class DispatchLog {NO_FUNCTION = 0, NOT_ALLOWED = 1, DONE = 2};
	class Module{
		public:
			typedef Module CLASSNAME;
			Module(SmartUppBot& _bot) : bot(_bot){}
			SmartUppBot& GetBot(){return bot;}

			virtual String GetName()const = 0;
			
			virtual String ToString()const{return "No information for this module";};
			
			virtual bool TestPrefixes(const String& prefix){
				for(const String& str : prefixes){
					if(str.IsEqual(prefix))
						return true;
				}
				return false;
			}
			
			DispatchLog DispatchCommand(ValueMap& payload, Command& dispatched);
			const Array<String>& GetPrefixes()const{return prefixes;}
			
			virtual void Help(ValueMap& payload, Command& dispatched) = 0;
			virtual void WhenReady(ValueMap& payload){}
			virtual void WhenError(ValueMap& payload){}
			virtual void WhenGuildStatus(ValueMap& payload){}
			virtual void whenChannelCreate(ValueMap& payload){}
			virtual void WhenVoiceStateCreate(ValueMap& payload){}
			virtual void WhenVoiceStateUpdate(ValueMap& payload){}
			virtual void WhenVoiceStateDelete(ValueMap& payload){}
			virtual void WhenVoiceSettingsUpdate(ValueMap& payload){}
			virtual void WhenVoiceConnectionStatus(ValueMap& payload){}
			virtual void WhenSpeakingStart(ValueMap& payload){}
			virtual void WhenSpeakingStop(ValueMap& payload){}
			virtual void WhenMessageCreate(ValueMap& payload){}
			virtual void WhenMessageUpdate(ValueMap& payload){}
			virtual void WhenMessageDelete(ValueMap& payload){}
			virtual void WhenNotificationCreate(ValueMap& payload){}
			virtual void WhenUnknown(ValueMap& payload){}
			
			virtual ~Module(){}
		protected:
			
			void RegisterFonction(Event<ValueMap&, Command&> cb, const String& commandName){
				Fonction& fn = handlers.Add(ToLower(commandName));
				fn.command = commandName;
				fn.event = cb;
			}
			void RegisterPrefix(const String& prefix){
				prefixes.Add(ToLower(prefix));
			}
		private:
			Array<String> prefixes;
			ArrayMap<String, Fonction> handlers;
			SmartUppBot& bot;
			friend SmartUppBot;
			ArrayMap<String, Fonction>& GetHandlers(){return handlers;}
	};
	
	class SmartUppBot{
		public:
			void Run();
			SmartUppBot(const String& _botID, const String& _botToken);
			~SmartUppBot();
			
			typedef SmartUppBot CLASSNAME;
					
			/*
			ValueMap GetChannel(const String& channelID);
			ValueMap GetChannelMessage(const String& channelID, const String& messageID);
			ValueMap GetChannelMessages(const String& channelID);
			
			ValueMap DeleteOwnReaction(const String& channelID, const String& messageID, const String& url_encoded_emoji);
			ValueMap DeleteUserReaction(const String& channelID, const String& messageID, const String& url_encoded_emoji, const String& userID);
			ValueMap GetReactions(const String& channelID, const String& messageID, const String& url_encoded_emoji, int limite=25);
			
			ValueMap DeleteAllReactions(const String& channelID, const String& messageID);
			ValueMap DeleteAllReactionsForEmoji(const String& channelID, const String& messageID, const String& url_encoded_emoji);
			
			ValueMap EditMessage(const String& channelID, const String& messageID, const String& content,  const ValueMap& embed_object, int flags, const ValueMap& allowed_mentions);
			*/
			
			ValueMap CreateReaction(const String& channelID, const String& messageID, const String& emojiName);
			ValueMap CreateMessage(const String& channel,const String& message);
			
			ValueMap DeleteMessage(const String& channelID, const String& messageID);
			ValueMap BulkDeleteMessages(const String& channelID, int numberMessage = 1);
			ValueMap BulkDeleteMessages(const String& channelID, const Vector<String>& messagesIDs);
			
			Time GetLaunchedTime()const{return time;}
			Time GetConnectedTime()const{return uptime;}
			const Guild& GetGuild()const{return guild;}
		
			template <class T, class... Args>
			bool CreateModule(Args&&... args){
				for(Module& m : modules){
					if(typeid(m) == typeid(T))
						return false;
				}
				T& mod = modules.Create<T>(*this, std::forward<Args>(args)...);
				LOG(AsString(GetSysTime()) + " | Module " + mod.GetName() +" have been created !");
				return true;
			}
			
			template <class T>
			bool RemoveModule(){
				for(int e = 0; e < modules.GetCount(); e++){
					if(typeid(modules[e]) == typeid(T)){
						LOG(AsString(GetSysTime()) + " | Module " + modules[e].GetName() +" have been removed !");
						modules.Remove(e,1);
						return true;
					}
				}
				return false;
			}
			
			const Array<Module>& GetModules()const{return modules;}
			
			void LoadConfig();
			void SaveConfig();
		protected:
			//All default command
			void Help(ValueMap& payload, Command& dispatched);
			void Uptime(ValueMap& payload, Command& dispatched);
			void DumpMembers(ValueMap& payload, Command& dispatched);
			void DumpRoles(ValueMap& payload, Command& dispatched);
			void DumpChannels(ValueMap& payload, Command& dispatched);
			void DumpEmojis(ValueMap& payload, Command& dispatched);
			void Delete(ValueMap& payload, Command& dispatched);
			
		private:
			Array<Module> modules;

			bool Initializing();
			bool Connecting(bool reset = true);
			bool Identifying();
			bool Resuming();
			
			void Listen();
			void InitThread(unsigned int session_id, unsigned int heartbeat_interval); //Init the connexion by setting Heartbeat thread and then call Identify procedure
			void CloseThread();
			
			void InitGuild(ValueMap& payload, const String& guildID); //Build the entire Guild item

			void Dispatch(ValueMap& payload); //Dispatcher
			void KeepAlive(unsigned int time); //function launch via a thread to keep alive the websocket
			void ApplyRateLimits(HttpRequest& req); //Wait until the next http request can be dispatched, it ensure discord api don't kick us because of high flow rate
			
			Array<Fonction> fonctions;
			DispatchLog CheckRight(const String& str, ValueMap& payload);
			
			String GetIDFromRoleName(const String& str);
			String GetIDFromUserName(const String& str);
			
			void FillRolesAllowed(Fonction& fonc, int foncCount, Value& vm);
			void FillUsersAllowed(Fonction& fonc, int foncCount, Value& vm);
			
			String GetNameByRoleID(const String& str);
			String GetNameByUserID(const String& str);
						
			String botID;
			String botToken;
			
			Guild guild;
			
			String gatewayAddr;
			String baseUrl = "https://discordapp.com";
			
			unsigned int sessionID = 0;
			unsigned int heartbeatInterval = 0;
			unsigned int lastSequenceNum = 0;
			
			bool keepRunning = true;
			bool shouldResume = false;
			bool WaitingForHeartBeatACK = false;
			
			Thread keepAliveThread;
			bool keepAliveStart = false;
			Mutex botMutex;
			
			HttpRequest discordRequest;
			WebSocket webSocket;
			WebSocket voiceSocket;
			
			Time time;
			Time uptime;
	};
}



#endif

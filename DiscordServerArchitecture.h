#ifndef _SmartUppBot2021_DiscordServerArchitecture_h_
#define _SmartUppBot2021_DiscordServerArchitecture_h_
namespace Upp{
	class Emoji : public Moveable<Emoji>{
		public:
			Emoji(const String& _ID, const String& _name, bool _requiere_colons, bool _managed, bool _animated, bool _available)
				: ID(_ID), name(_name), requiere_colons(_requiere_colons), managed(_managed), animated(_animated), available(_available){}
			
			String GetID()const{return ID;}
			String GetName()const{return name;}
			
			bool IsRequiereColons()const{return requiere_colons;}
			bool IsManaged()const{return managed;}
			bool IsAnimated()const{return animated;}
			bool IsAvailable()const{return available;}
			
		private:
			String ID;
			String name;
			
			bool requiere_colons;
			bool managed;
			bool animated;
			bool available;
	};
	
	class Role : public Moveable<Role>{
		public:
			Role(const String& _ID, const String& _name, bool _hoist, bool _managed, bool _mentionable)
					: ID(_ID), name(_name), hoist(_hoist), managed(_managed), mentionable(_mentionable){}
			
			String GetID()const{return ID;}
			String GetName()const{return name;}
			
			bool IsHoist()const{return hoist;}
			bool IsManaged()const{return managed;}
			bool IsMentionable()const{return mentionable;}
			
		private:
			String ID;
			String name;
			
			bool hoist;
			bool managed;
			bool mentionable;
	};
	
	class Member : public Moveable<Member>{
		public:
			Member(const String& _ID, const String& _username, const String& _descriminator)
				: ID(_ID), username(_username), descriminator(_descriminator){}
			
			String GetID()const{return ID;}
			String GetUsername()const{return username;}
			String GetDescriminator()const{return descriminator;}
		private:
			String ID;
			String username;
			String descriminator;
	};
	
	enum class ChannelType{
		GUILD_TEXT = 0,
		DM = 1,
		GUILD_VOICE = 2,
		GROUP_DM = 3,
		GUILD_CATEGORY = 4,
		GUILD_NEWS = 5,
		GUILD_STORE = 6
	};
	
	class Channel : public Moveable<Channel>{
		public:
			Channel(const String& _ID, ChannelType _type, const String& _guild_ID, const String& _name)
					: ID(_ID), type(_type), guild_ID(_guild_ID), name(_name){}
			
			String GetID()const{return ID;}
			ChannelType GetType()const{return type;}
			String GetGuildID()const{return guild_ID;}
			String GetName()const{return name;}
			
		private:
			String ID;
			ChannelType type;
			String guild_ID;
			String name;
	};
	
	class Guild{
		public:
			Guild(){}
			
			void Clear(){ ID=""; name=""; icon=""; description=""; emojis.Clear(); roles.Clear(); channels.Clear(); members.Clear(); loaded = false;}
			
			String GetID()const{return ID;}
			String GetName()const{return name;}
			String GetIcon()const{return icon;}
			String GetDescription()const{return description;}
			
			const Vector<Emoji>& GetEmojis()const{return emojis;}
			const Vector<Role>& GetRoles()const{return roles;}
			const Vector<Channel>& GetChannels()const{return channels;}
			const Vector<Member>& GetMembers()const{return members;}
			
			bool IsLoaded()const{return loaded;}
			
			bool Build(const ValueMap& guildObject, const ValueMap& channelsObject, const ValueMap& membersObject){
				try{
					ID = ~guildObject["id"];
					name = ~guildObject["name"] ;
					icon = ~guildObject["icon"];
					description = ~guildObject["description"];
					
					Value emojisArray = guildObject["emojis"];
					for(int i = 0; i < emojisArray.GetCount(); i++) {
						emojis.Create(~emojisArray[i]["id"],":" +~emojisArray[i]["name"] +":",emojisArray[i]["require_colons"],emojisArray[i]["managed"],emojisArray[i]["animated"],emojisArray[i]["available"]);
					}
					Value rolesArray = guildObject["roles"];
					for(int i = 0; i < rolesArray.GetCount(); i++) {
						roles.Create(~rolesArray[i]["id"],~rolesArray[i]["name"],rolesArray[i]["hoist"],rolesArray[i]["managed"],rolesArray[i]["mentionable"]);
					}
					for(int i = 0; i < channelsObject.GetCount(); i++) {
						channels.Create(~channelsObject[i]["id"],(ChannelType) int(channelsObject[i]["type"]), ~channelsObject[i]["guild_id"], ~channelsObject[i]["name"]);
					}
					for(int i = 0; i < membersObject.GetCount(); i++){
						members.Create(membersObject[i]["user"]["id"],membersObject[i]["user"]["username"], membersObject[i]["user"]["discriminator"]);
					}

					loaded = true;
				}catch(Exc& exception){
					LOG(AsString(GetSysTime()) + " | Guild::Build have failled : " + exception);
					loaded = false;
				}
				return loaded;
			}
			
			void AddMembers(const ValueMap& membersObject){
				for(int i = 0; i < membersObject.GetCount(); i++){
					members.Create(membersObject[i]["user"]["id"],membersObject[i]["user"]["username"], membersObject[i]["user"]["discriminator"]);
				}
			}
			
		private:
			String ID;
			String name;
			String icon;
			String description;
			
			Vector<Emoji> emojis;
			Vector<Role> roles;
			Vector<Channel> channels;
			Vector<Member> members;
			
			bool loaded;
	};
}
#endif

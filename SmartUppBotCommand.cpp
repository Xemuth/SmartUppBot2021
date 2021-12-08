#include "SmartUppBot.h"
#include "AllEmoji.h"

namespace Upp{
ValueMap SmartUppBot::CreateMessage(const String& channelID,const String& message){
	discordRequest.New();
    Json json("content", message);
    String response;
    {
	    Mutex::Lock __(botMutex);
	    response =
	        discordRequest.Url(baseUrl)
	           .Path("/api/channels/" + channelID + "/messages")
	           .POST()
	           .Post(json)
	           .Execute();
    }
    
           
    ApplyRateLimits(discordRequest);
    discordRequest.Close();
    return pick(ParseJSON(response));
}

ValueMap SmartUppBot::CreateReaction(const String& channelID, const String& messageID, const String& emojiName){
	int position;
	String response;
	String realEmojiName = Replace(emojiName,Vector<String>{":"},Vector<String>{""});
	realEmojiName = ":" + realEmojiName + ":";
	
	discordRequest.New();
	position = emojis.Find(realEmojiName);
	
	if(position != -1){
		Mutex::Lock __(botMutex);
	    response = discordRequest.Url(baseUrl)
        .Path("/api/channels/" + channelID + "/messages/" + messageID + "/reactions/"+ emojis[position] +"/@me")
        .Method(HttpRequest::METHOD_PUT)
        .Execute();
	}else{
		for(const Emoji& emo : guild.GetEmojis()){
			if(emo.GetName().IsEqual(realEmojiName)){
				Mutex::Lock __(botMutex);
				response = discordRequest.Url(baseUrl)
		        .Path("/api/channels/" + channelID + "/messages/" + messageID + "/reactions/"+ UrlEncode(emo.GetName() + emo.GetID()) +"/@me")
		        .Method(HttpRequest::METHOD_PUT)
		        .Execute();
				break;
			}
		}
	}
    ApplyRateLimits(discordRequest);
    discordRequest.Close();
    return pick(ParseJSON(response));
}

ValueMap SmartUppBot::DeleteMessage(const String& channelID, const String& messageID){
	discordRequest.New();
	String response;
    {
		Mutex::Lock __(botMutex);
		response = discordRequest.Url(baseUrl)
           .Path("/api/channels/" + channelID + "/messages/" + messageID)
           .Method(HttpRequest::METHOD_DELETE)
           .Execute();
    }
    ApplyRateLimits(discordRequest);
    discordRequest.Close();
    return pick(ParseJSON(response));
}

ValueMap SmartUppBot::BulkDeleteMessages(const String& channelID, int numberMessage){
	discordRequest.New();
	String response;
	{
		Mutex::Lock __(botMutex);
		response = discordRequest.Url(baseUrl)
           .Path("/api/channels/" + channelID + "/messages")
           .Method(HttpRequest::METHOD_GET)
           .Execute();
	}
    
    Value messages = ParseJSON(response);
    Vector<String> toDelete;
    if(numberMessage > messages.GetCount()) numberMessage = messages.GetCount();
	for(int i = 0; i < numberMessage; i++) {
		toDelete << (~messages[i]["id"]);
	}
    ApplyRateLimits(discordRequest);
    discordRequest.Close();

    return pick(BulkDeleteMessages(channelID,toDelete));
}

ValueMap SmartUppBot::BulkDeleteMessages(const String& channelID, const Vector<String>& messagesIDs){
	discordRequest.New();
    
    JsonArray mess;
    for(const String& str : messagesIDs){
		mess << str;
    }
    Json json("messages", mess);
    String response;
    {
		Mutex::Lock __(botMutex);
		response = discordRequest.Url(baseUrl)
           .Path("/api/channels/" + channelID + "/messages/bulk-delete")
           .Method(HttpRequest::METHOD_POST)
           .Post(json)
           .Execute();
    }
    ApplyRateLimits(discordRequest);
    discordRequest.Close();
    return pick(ParseJSON(response));
}
}
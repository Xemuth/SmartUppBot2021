#include "SmartUppBot.h"

namespace Upp{
	
void SmartUppBot::Uptime(ValueMap& payload, Command& dispatched){
	String strTime;
    Time buffTime = GetSysTime();
    strTime << "```SmartUppBot have been launched for : " << DisplayEllaspedtime(TimeFromUTC(buffTime - GetLaunchedTime()))  <<"\nConnected to discord gateway for : " << DisplayEllaspedtime(TimeFromUTC(buffTime - GetConnectedTime())) <<"```";
	CreateMessage(payload["d"]["channel_id"], strTime);
}

void SmartUppBot::Help(ValueMap& payload, Command& dispatched){
	String serverHelp;
	serverHelp << "```--------------" << version <<"--------------\n";
	serverHelp << "!Uptime -> Affiche le temps de fonctionnement du bot\n";
	serverHelp << "!Delete(nombre entier entre 1 et 50) -> Supprime le nombre spécifié de message sur le channel\n";
	serverHelp << "!DumpMembers -> Affiche tous les membres du serveur\n";
	serverHelp << "!DumpRoles -> Affiche tous les roles du serveur\n";
	serverHelp << "!DumpChannels -> Affiche tous les channels du serveur\n";
	serverHelp << "!DumpEmojis -> Affiche tous les emojis custom du serveur\n";
	serverHelp << "!Save -> Sauvegarde l'actuel configuration dans un fichier JSON\n";
	serverHelp << "!Reload -> Recharge la configuration du serveur\n";
	serverHelp << "*************************************************************\n";
	serverHelp << "Voici la liste de tous les modules présent sur cette installation de SmartUppBot :\n";
	for(int e = 0; e < GetModules().GetCount(); e++){
		serverHelp << GetModules()[e].GetName() << " - prefixes: " << GetModules()[e].GetPrefixes().ToString() <<" - " << GetModules()[e].ToString() <<"\n";
	}
	serverHelp << "```";
	CreateMessage(payload["d"]["channel_id"],serverHelp);
}

void SmartUppBot::DumpMembers(ValueMap& payload, Command& dispatched){
	String ServerInfo;
	int memberCount = GetGuild().GetMembers().GetCount();
	ServerInfo <<"```---------" + AsString(memberCount) +" members------------\n";
	for(int e = 0; e < memberCount; e++){
		ServerInfo << AsString(e) << " - " << GetGuild().GetMembers()[e].GetUsername() +"#" + GetGuild().GetMembers()[e].GetDescriminator()<<"\n";
	}
	ServerInfo << "```";
	CreateMessage(payload["d"]["channel_id"],ServerInfo);
}

void SmartUppBot::DumpRoles(ValueMap& payload, Command& dispatched){
	String ServerInfo;
	int roleCount = GetGuild().GetRoles().GetCount();
	ServerInfo <<"```---------" + AsString(roleCount) +" roles------------\n";
	for(int e = 0; e < roleCount; e++){
		ServerInfo << AsString(e) << " - " << GetGuild().GetRoles()[e].GetName() <<"\n";
	}
	ServerInfo << "```";
	CreateMessage(payload["d"]["channel_id"],ServerInfo);
}

void SmartUppBot::DumpChannels(ValueMap& payload, Command& dispatched){
	String ServerInfo;
	int chanCount = GetGuild().GetChannels().GetCount();
	ServerInfo <<"```---------" + AsString(chanCount) +" roles------------\n";
	for(int e = 0; e < chanCount; e++){
		ServerInfo << AsString(e) << " - " << GetGuild().GetChannels()[e].GetName() <<"\n";
	}
	ServerInfo << "```";
	CreateMessage(payload["d"]["channel_id"],ServerInfo);
}

void SmartUppBot::DumpEmojis(ValueMap& payload, Command& dispatched){
	String ServerInfo;
	int emoCount = GetGuild().GetEmojis().GetCount();
	ServerInfo <<"```---------" + AsString(emoCount) +" roles------------\n";
	for(int e = 0; e < emoCount; e++){
		ServerInfo << AsString(e) << " - " << GetGuild().GetEmojis()[e].GetName() <<"\n";
	}
	ServerInfo << "```";
	CreateMessage(payload["d"]["channel_id"],ServerInfo);
}

void SmartUppBot::Delete(ValueMap& payload, Command& dispatched){
	if(dispatched.values.GetCount() > 0 && dispatched.values[0].Is<double>() && dispatched.values[0].Get<double>() >= 1){
		BulkDeleteMessages(payload["d"]["channel_id"],(int) dispatched.values[0].Get<double>());
	}else{
		CreateMessage(payload["d"]["channel_id"],"Veuillez passer le nombre de message à delete : !delete(1 - 100)");
	}
}

}
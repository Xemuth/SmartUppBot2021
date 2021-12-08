#ifndef _SmartUppBot2021_ModuleInfo_h_
#define _SmartUppBot2021_ModuleInfo_h_
#include "SmartUppBot.h"

namespace Upp{

class Info : public Module{
	public:
		typedef Info CLASSNAME;
		
		Info(SmartUppBot& _bot) : Module(_bot){
			//REGISTER_NO_PREFIX();
			REGISTER_PREFIX("info");

		//	REGISTER(Delete);
		//	REGISTER(Help);
		}
		
		virtual String GetName()const{return "Info";}
		virtual String ToString(){return "Return multiple information about the bot";};
		
	/*	void Help(ValueMap& payload, Command& dispatched){
			String serverHelp;
			serverHelp << "```";
			serverHelp << "!info Help -> Affiche le temps de fonctionnement du bot\n";
			serverHelp << "!info Delete(nombre entier entre 1 et 50) -> Supprime le nombre spécifié de message sur le channel\n";
			serverHelp << "```";
			GetBot().CreateMessage(payload["d"]["channel_id"],serverHelp);
		}
		
		void Delete(ValueMap& payload, Command& dispatched){
			if(dispatched.values.GetCount() > 0 && dispatched.values[0].Is<double>() && dispatched.values[0].Get<double>() >= 1){
				GetBot().BulkDeleteMessages(payload["d"]["channel_id"],(int) dispatched.values[0].Get<double>());
			}else{
				GetBot().CreateMessage(payload["d"]["channel_id"],"Veuillez passer le nombre de message à delete : !delete(1 - 100)");
			}
		}
		*/
};
}

#endif

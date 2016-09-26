#include <fstream>
#include <sstream>
#include "globalVariables.hpp"

void loadConfig(const char *cfgPath, unsigned int &windowX, unsigned int &windowY, unsigned int &windowW, unsigned int &windowH){

	std::ifstream serverConfig(cfgPath);
	std::string line;

	if(serverConfig.is_open()){
		while(serverConfig.good()){

			getline(serverConfig, line);

			// Remove any comments from the line
			unsigned int commentPos = line.find("//");
			if(commentPos != std::string::npos){
				line.erase(commentPos);
			}
			// Remove any tabs from the line
			for(unsigned int d = 0; d < line.length(); d++){
				if(line.at(d) == '\t'){
					line.erase(d, 1);
				}
			}

			if(line.length() > 10){

				std::string commandSubstr = line.substr(0, 10);

				if(commandSubstr == "windowX = "){
					std::istringstream(line.substr(10)) >> windowX;

				}else if(commandSubstr == "windowY = "){
					std::istringstream(line.substr(10)) >> windowY;

				}else if(commandSubstr == "windowW = "){
					std::istringstream(line.substr(10)) >> windowW;

				}else if(commandSubstr == "windowH = "){
					std::istringstream(line.substr(10)) >> windowH;

				}


				if(line.length() > 11){

					commandSubstr = line.substr(0, 11);

					if(commandSubstr == "resetKey = "){
						std::istringstream(line.substr(11)) >> resetKey;

					}else if(commandSubstr == "pauseKey = "){
						std::istringstream(line.substr(11)) >> pauseKey;

					}else if(commandSubstr == "abortKey = "){
						std::istringstream(line.substr(11)) >> abortKey;

					}

				}

			}

		}

		serverConfig.close();
		printf("Config loaded.\n");

	}else{
		printf("Specified config path is invalid. No config loaded.\n");
	}

}

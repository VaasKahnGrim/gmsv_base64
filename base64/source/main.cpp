#include "GarrysMod/Lua/Interface.h"
#include <stdio.h>
#include "stdint.h"
#include <string>
#include "utilities.cpp"

using namespace GarrysMod::Lua;

static uint32_t getDecodedSize(const char* input)
{
	size_t length = strlen(input);
	if (length == 0) return 0;

	const char* last = input + length - 4;
	const char* pad = strchr(last, '=');
	
	int32_t padLength = pad == NULL ? 0 : (last - pad);
	return (length / 4) * 3 - padLength;
}

void decodeblock(char *input,char *output,int oplen){
	char decodedstr[5] = "";

	decodedstr[0] = input[0] << 2 | input[1] >> 4;
	decodedstr[1] = input[1] << 4 | input[2] >> 2;
	decodedstr[2] = input[2] << 6 | input[3] >> 0;

	strncat(output,decodedstr,oplen - strlen(output));
}


char BASE64CHARSET[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

void C_Base64Decode(const char *input, char *output, int oplen){
	char decoderinput[5]	= "";
	char* charval			= 0;
	int index 				= 0;
	int asciival 			= 0;
	int computeval			= 0;
	int iplen 				= 0;

	iplen = strlen(input);
	while(index < iplen){
		asciival = (int)input[index];
		if(asciival == '='){
			decodeblock(decoderinput,output,oplen);
			break;
		}else{
			charval = strchr(BASE64CHARSET,asciival);

			if(charval){
				decoderinput[computeval] = charval - BASE64CHARSET;
				computeval = (computeval + 1) % 4;
				
				if(computeval == 0){
					decodeblock(decoderinput,output,oplen);

					decoderinput[0] = decoderinput[1] =
					decoderinput[2] = decoderinput[3] = 0;
				}
			}
		}
		index++;
   }
}

LUA_FUNCTION(Base64Decode){
	LUA->CheckType(1,GarrysMod::Lua::Type::STRING);
	const char* Input = LUA->GetString(1);
	uint32_t decodedSize = getDecodedSize(Input);

	if (decodedSize == 0)
	{
		LUA->PushString("");
		return 1;
	}

	char* Output = new char[decodedSize];
	memset(Output, 0, decodedSize);
	C_Base64Decode(Input, Output, decodedSize);
	LUA->PushString((const char*)Output);
	delete[] Output;
	return 1;
}

void Vaas_LongMath_init(GarrysMod::Lua::ILuaBase* LUA){
	printMessage(LUA,"Vaas Base64Decode module loaded.\n",0,255,0);
	LUA->PushSpecial(GarrysMod::Lua::SPECIAL_GLOB);
		LUA->PushCFunction(Base64Decode);
		LUA->SetField(-2,"Base64Decode");
	LUA->Pop();
}

// Called when you module is opened
GMOD_MODULE_OPEN(){
	Vaas_LongMath_init(LUA);
	return 0;
}

// Called when your module is closed
GMOD_MODULE_CLOSE(){
	printMessage(LUA,"Vaas Base64Decode module nolonger in use, unloading.\n",0,255,0);
	return 0;
}

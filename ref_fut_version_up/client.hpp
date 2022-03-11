#pragma once

#include "../../../base/log.hpp"
#include "../../../base/baseFun.hpp"

#include <fstream>
#include <vector>

using namespace pp;
using namespace std;

string tool_version	=	"1.0";

string getFPGATypeCmd = "/home/user0/intelFPGA/18.0/qprogrammer/bin/jtagconfig ";
string upHWVersionCmd = "./fpga -m  1 -b mg5 ";

struct UpVersionInfo  
{
	string	 SrvIP;
	string	 NeedHwVersion;
	string	 NeedSwVersion;
	string   NowSwVersion;
	string	 FPGAType;
	string   ProductDir;
	string	 InstallDir;
};


UpVersionInfo							g_xmlParam;
CLog									g_log("rem_fut_version_up", true);


// ����Ӳ��
void upHW();
// ִ��sql�ļ�
int dbParamUpdate(int type);

void runCmdLine(const string cmd);
void getExecResultStrV(vector<string>& resultLineV, string file);

bool findKeyInfo(vector<string>& resultLineV, string key);
//void loadInfoToLog(vector<string>& resultLineV, CLog& log);
// ȥ������Ҫ�İ汾sql�ű�
void clearNoNeedVerList(vector<string>& resultLineV, string nowVer, string needVer);


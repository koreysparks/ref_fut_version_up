#include "client.hpp"

int main(const int argc, const char* argv[])
{
	string type = argv[1];
	// 中间输出文件
	string tmpResult = "tmpResult";
	// 重定向结果文件
	vector<string> execResultV;
	runCmdLine("cat rem_install.conf > tmpResult");
	getExecResultStrV(execResultV, tmpResult);
	for(size_t i = 0; i < execResultV.size(); ++i)
	{
		if(string::npos != execResultV[i].find("REM_VERSION="))
		{
			vector<string> paramLineV;
			splitStr(paramLineV, execResultV[i], '=');
			g_xmlParam.NeedSwVersion = paramLineV[1];
		}

		if(string::npos != execResultV[i].find("REM_HW_VERSION="))
		{
			vector<string> paramLineV;
			splitStr(paramLineV, execResultV[i], '=');
			if(2 == paramLineV.size())
			{
				g_xmlParam.NeedHwVersion = paramLineV[1];
			}
		}

		if(string::npos != execResultV[i].find("NOW_VERSION="))
		{
			vector<string> paramLineV;
			splitStr(paramLineV, execResultV[i], '=');
			g_xmlParam.NowSwVersion = paramLineV[1];
		}


		if(string::npos != execResultV[i].find("DB_IP="))
		{
			vector<string> paramLineV;
			splitStr(paramLineV, execResultV[i], '=');
			g_xmlParam.SrvIP = paramLineV[1];
		}

		if(string::npos != execResultV[i].find("REM_DIR="))
		{
			vector<string> paramLineV;
			splitStr(paramLineV, execResultV[i], '/');
			g_xmlParam.ProductDir = string("/home/user0/") + paramLineV[1];
		}

		if(string::npos != execResultV[i].find("INST_INSTALL_PACKAGE="))
		{
			vector<string> paramLineV;
			splitStr(paramLineV, execResultV[i], '/');
			g_xmlParam.InstallDir = string("/home/user0/") + paramLineV[1];
		}
	}

	// 更新硬件
	if("up_hw" == type)
	{
		upHW();
	}

	if("up_db" == type)
	{
		// 升级 2 或 3 效果一样 
		dbParamUpdate(2);
	}

	if("back_db" == type)
	{
		// 回退
		dbParamUpdate(4);
	}

	return 0;
}

void upHW()
{
	// 中间输出文件
	string tmpResult = "tmpResult";
	// 重定向结果文件
	vector<string> execResultV;
	if(g_xmlParam.NeedHwVersion.empty())
	{
		g_log.log("warn hw file conf is empty, can not update hw");
		return;
	}

	// 拉低 // 支持后续产品 例如rem_two rem_three 
	runCmdLine( ("cd " + g_xmlParam.ProductDir + "/drive; chmod +x *; ./unload_driver").c_str() );
	runCmdLine( "cd /home/user0/pcie_cfg; chmod +x *; ./run_driver");
	runCmdLine( "lspci |grep Eth > tmpResult");
	getExecResultStrV(execResultV, tmpResult);
	if(!findKeyInfo(execResultV, "rev ff"))
	{
		// 拉低失败
		g_log.log(tmpResult.c_str());
		g_log.log("error set fpga sig low failed can not update hw");
		return;
	}

	// 升级硬件
	runCmdLine("cd /home/user0/hwfile_file_burn/xof/; mv *.xof ../");
	runCmdLine( ( string("cp ") + g_xmlParam.NeedHwVersion + 
		" /home/user0/hwfile_for_burn/xof; chmod +x /home/user0/hwfile_file_burn/xof/*").c_str());
	// 获取usb配置
	runCmdLine(getFPGATypeCmd + " > tmpResult");
	getExecResultStrV(execResultV, tmpResult);
	for(size_t i = 0; i < execResultV.size(); ++i)
	{
		const char* res = strstr(execResultV[i].c_str(), "USB");
		if(res)
		{
			runCmdLine(string("echo ") + string(res) + " > /home/user0/hwfile_for_burn/FPGA_burn/usbblaster_name.txt");
		}
	}

	if(!findKeyInfo(execResultV, "A9"))
	{
		g_xmlParam.FPGAType = "de5";
	}


	upHWVersionCmd = string("cd /home/user0/hwfile_for_burn/FPGA_burn; ./fpga -m 1 -b ") + g_xmlParam.FPGAType;
	runCmdLine((upHWVersionCmd  + " > " + g_xmlParam.InstallDir + "/tmpResult ").c_str());
	g_log.log(tmpResult.c_str());
	getExecResultStrV(execResultV, tmpResult);
	if(!findKeyInfo(execResultV, "Successful"))
	{
		g_log.log("error up hw version failed, please check failed reason in log file");
		return;
	}
	else
	{
		g_log.log("info up hw version successd now start up sw version");
	}
}

void runCmdLine(const string cmd)
{
	g_log.log(cmd.c_str());
	system( cmd.c_str());
}

int dbParamUpdate(int type)
{
	string sqlDir;
	if(2 == type)
	{
		sqlDir = g_xmlParam.InstallDir + "/" + g_xmlParam.NeedSwVersion + "/server_setup_pack/fut_db_files/up_fut_db_files";
		runCmdLine( (string("cd ") + g_xmlParam.NeedSwVersion + 
			"; unzip -o -q server_setup_pack.zip; cd " + sqlDir + ";ls > " + g_xmlParam.InstallDir + "/tmpResult").c_str());
	}
	else
	{
		sqlDir = g_xmlParam.InstallDir + "/" + g_xmlParam.NowSwVersion + "/server_setup_pack/fut_db_files/back_fut_db_files";
		runCmdLine( (string("cd ") + g_xmlParam.NowSwVersion + 
			"; unzip -o -q server_setup_pack.zip; cd " + sqlDir + ";ls > " + g_xmlParam.InstallDir + "/tmpResult").c_str());
	}
	
	// 中间输出文件
	string tmpResult = "tmpResult";
	// 重定向结果文件
	vector<string> versionNumList; // 版本号列表 中间跨度版本的sql都需要执行
	getExecResultStrV(versionNumList, tmpResult);

	g_xmlParam.NowSwVersion += ".sql";
	g_xmlParam.NeedSwVersion += ".sql";

	clearNoNeedVerList(versionNumList, g_xmlParam.NowSwVersion, g_xmlParam.NeedSwVersion);
	string sqlRun = string("cd ") + sqlDir;
	// 对于每一个版本的sql脚本
	for(size_t i = 0; i < versionNumList.size(); ++i)
	{
		if(versionNumList[i].empty())
		{
			// 跳过区间外的版本
			continue;
		}

		sqlRun += string(";mysql -h") + g_xmlParam.SrvIP + " -ushengli -pshengli0 < " + versionNumList[i];
		runCmdLine((string("cat ") + sqlDir + "/" + versionNumList[i]).c_str());
	}

	runCmdLine( sqlRun.c_str());	
	return 0;
}

void getExecResultStrV(vector<string>& resultLineV, string file)
{
	resultLineV.clear();
	string res;
	ifstream resultLine(file.c_str(), ios::in|ios::app);
	while(getline(resultLine, res))
	{
		resultLineV.push_back(res);
		res.clear();
	}
}

bool findKeyInfo(vector<string>& resultLineV, string key)
{
	for(size_t i = 0; i< resultLineV.size(); ++i)
	{
		if(string::npos != resultLineV[i].find(key))
		{
			return true;
		}
	}

	return false;
}

//void loadInfoToLog(vector<string>& resultLineV, CLog& log)
//{
//	for(size_t i = 0; i< resultLineV.size(); ++i)
//	{
//		g_log.log("info %s", resultLineV[i].c_str());
//	}
//}

void clearNoNeedVerList(vector<string>& resultLineV, string nowVer, string needVer)
{
	// 不同系列的版本号比较ascii 认为 one > 4.xx ultra > 3.xx，当前大版本号变更符合这个逻辑
	if(nowVer < needVer)
	{
		// 判断为版本升级
		for(size_t i = 0; i < resultLineV.size(); ++i)
		{	// 正常的版本号比较           // 正常情况下不会出现
			if(resultLineV[i] <= nowVer || resultLineV[i] > needVer)
			{
				resultLineV[i] = "";
			}
		}
	}
	else if(nowVer > needVer)
	{
		// 判断为版本回退
		for(size_t i = 0; i < resultLineV.size(); ++i)
		{	// 理论上不会出现			 // 正常的版本号比较
			if(resultLineV[i] > nowVer || resultLineV[i] <= needVer)
			{
				resultLineV[i] = "";
			}
		}
	}
	else
	{
		g_log.log("error version sql file clear type error");
	}
}

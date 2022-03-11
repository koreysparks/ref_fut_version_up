#! /bin/bash

. rem_install.conf
chmod +x *.sh

# 解压程序包
cd ${INST_SOURCE_DIR}/
unzip -o -q server_setup_pack.zip

# 更新程序包路径
echo ${SERVER_TAG_DIR}

# 全新安装数据库脚本目录
echo ${DB_SOURCE}

# 如果是全新安装，则导入寄存器配置，创建目录，导入数据库脚本
if [ ${NEW_INSTALL} -eq 1 ]; then
	
	# 判断是否已存在REM系统，若存在，退出安装，需手工删除数据库以后才能继续安装
	mysql -h${DB_IP} -u${DB_USER} -p${DB_PASSWD} -e "show databases" | grep -w ${CONFIG_DB}
	if [ $? -eq 0 ]; then
		echo -e "\033[31m rem system exists, can not be installed. please check it. If you want to install new rem, please drop old config db or change new config db name first. \033[0m"
      	exit 1
	else
		echo -e "\033[32m rem system does not exist, start new install now. \033[0m"
	
	if [ ${IS_KIWI} -eq 1 ] && [ ${MAC_ADDR} == "00:00:00:00:00:00" ]; then
		echo -e "\033[31m A hardware system will be installed. Please set mac address in rem_install.conf first!!! \033[0m"
		exit 2
	fi	
		cd ${INST_INSTALL_PACKAGE}
		# 替换数据库脚本，配置文件内容
		echo "start to run edit_file.sh."
		./edit_file.sh
		
		# 各服务路径创建
		./make_dir.sh
		
		#拷贝配置文件
		./copy_config_file.sh
		
		# 拷贝整体运行脚本
		./copy_kiwi_script.sh
		
		#拷贝eqs_probe脚本
		./copy_eqs_probe.sh
		
		# 数据库脚本导入
		./import_db.sh
		
	fi
	
	# 删除与当前配置的交易所无关的内容
	EX_ARR=( 102 103 104 105 106 107 )
	for EX in ${EX_ARR[*]}
	do
	if [[ ${EXCHANGE} =~ ${EX} ]]; then
		echo -e "\033[32m this env contains ${EX}\033[0m"
	else
		mysql -h${DB_IP} -u${DB_USER} -p${DB_PASSWD} ${CONFIG_DB} <${INST_INSTALL_PACKAGE}/delete_${EX}.sql
	fi
	done

	# 金交所标志
	if [[ ${EXCHANGE} =~ 107 ]]; then
		mysql -h${DB_IP} -u${DB_USER} -p${DB_PASSWD}  -e "UPDATE ${CONFIG_DB}.t_global_settings SET setting_value='Y' WHERE setting_key='EES_SGE'"
		mysql -h${DB_IP} -u${DB_USER} -p${DB_PASSWD}  -e "DELETE FROM ${CONFIG_DB}.t_symbol_category WHERE exchange_id != '107'"
		mysql -h${DB_IP} -u${DB_USER} -p${DB_PASSWD}  -e "DELETE FROM ${CONFIG_DB}.t_china_future_instrument WHERE symbol > 4"
	else
		mysql -h${DB_IP} -u${DB_USER} -p${DB_PASSWD}  -e "UPDATE ${CONFIG_DB}.t_global_settings SET setting_value='N' WHERE setting_key='EES_SGE'"	
	fi
	
	# 中金所，需要添加一个软连接
	if [[ ${EXCHANGE} =~ 102 ]]; then
		ln -sf /usr/lib64/libssl.so.10 /usr/lib64/libssl.so.6
	fi
fi

# 升级前备份数据库 不备份历史库
./rem_backup_db.sh

# 升级或回退硬件版本
if [ ${NEW_INSTALL} -eq 3 || ${NEW_INSTALL} -eq 4 ]; then	
	./rem_fut_version_up up_hw
fi
# 拷贝
cd ${INST_INSTALL_PACKAGE}
./copy_rem_file.sh



chmod -R 777 ${REM_DIR}

# update DB
cd ${DB_UPDATE_DIR}
./ees_dbu

# 版本升级 执行升级脚本 添加默认配置
if [ ${NEW_INSTALL} -eq 2  || ${NEW_INSTALL} -eq 3 ]
	./rem_fut_version_up up_db
fi

# 版本回退 执行回退脚本 删除默认配置
if [ ${NEW_INSTALL} -eq 4 ]
	./rem_fut_version_up back_db
fi
	
cd ${INST_SOURCE_DIR}
rm -rf server_setup_pack

# 导入合约列表和结算价
if [ ${NEW_INSTALL} -eq 1 ] && [[ ${DB_IP} =~ '10.1.5' ]]; then
	cd ${INST_INSTALL_PACKAGE}
	./import_sm_and_closrpt.sh
fi
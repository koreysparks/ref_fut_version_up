#! /bin/bash
. rem_install.conf

# get datetime
sys_date=$(date +%Y%m%d)
sys_time=$(date +%H%M%S)

# create dest datetime dir
TAG_DIR=${DB_BACKUP_DIR}
dest_dir=$TAG_DIR/${sys_date}
mkdir -p ${TAG_DIR}
mkdir -p ${dest_dir}

REM_DB_CONN_STR=" -h${DB_IP} -u${DB_USER} -p${DB_PASSWD} "

# 备份数据库
# 备份 ees_config
ees_dump=${CONFIG_DB}_${sys_date}_${sys_time}.sql
ees_zip=${dest_dir}/${CONFIG_DB}_dump_${sys_date}_${sys_time}.zip

#set -x
mysqldump ${REM_DB_CONN_STR} ${CONFIG_DB}  > ${ees_dump}
zip -r ${ees_zip} ${ees_dump}
rm ${ees_dump}
#set +x

# 备份 ees_trading_data
sys_time=$(date +%H%M%S)
td_dump=${TD_DB}_${sys_date}_${sys_time}.sql
td_zip=${dest_dir}/${TD_DB}_dump_${sys_date}_${sys_time}.zip

#set -x
mysqldump ${REM_DB_CONN_STR} ${TD_DB} > ${td_dump}
zip -r ${td_zip} ${td_dump}
rm ${td_dump}
#set +x

# 备份 ees_history_trading_data
#sys_time=$(date +%H%M%S)
#td_his_dump=${HIS_DB}_${sys_date}_${sys_time}.sql
#td_his_zip=${dest_dir}/${HIS_DB}_dump_${sys_date}_${sys_time}.zip

#set -x
#mysqldump ${REM_DB_CONN_STR} ${HIS_DB} > ${td_his_dump}
#zip -r ${td_his_zip} ${td_his_dump}
#rm ${td_his_dump}
#set +x

# 备份 ees_risk_data
sys_time=$(date +%H%M%S)
risk_dump=${RISK_DB}_${sys_date}_${sys_time}.sql
risk_zip=${dest_dir}/${RISK_DB}_dump_${sys_date}_${sys_time}.zip

#set -x
mysqldump ${REM_DB_CONN_STR} ${RISK_DB} > ${risk_dump}
zip -r ${risk_zip} ${risk_dump}
rm ${risk_dump}
#set +x

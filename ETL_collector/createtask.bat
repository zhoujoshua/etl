@echo off 
rem SCHTASKS /Create [/S system [/U username [/P password]]]
rem     [/RU username [/RP password]] /SC schedule [/MO modifier] [/D day]
rem     [/I idletime] /TN taskname /TR taskrun [/ST starttime] [/M months]
rem     [/SD startdate] [/ED enddate]
rem 
rem 描述:
rem     启用管理员以在本地或远程系统上
rem     创建计划任务。
rem 
rem 参数列表:
rem     /S           system            指定要连接到的远程
rem                                    系统。如果省略这个
rem                                    系统参数，默认是本地
rem                                    系统。
rem 
rem     /U           username          指定命令执行的
rem                                    用户上下文。
rem 
rem     /P           password          指定给定用户上下文
rem                                    的密码。
rem 
rem     /RU          username          指定任务运行的用户
rem                                    帐户 (用户上下文)。
rem                                    对于系统帐户，有效值是 ""，
rem                                    "NT AUTHORITY\SYSTEM" 或
rem                                    "SYSTEM"。
rem 
rem     /RP          password          指定用户密码。
rem                                    要提示输入密码，值
rem                                    必须是 "*" 或没有。
rem                                    密码将不会对系统帐户
rem                                    起作用。
rem 
rem     /SC          schedule          指定计划频率。
rem                                    有效计划任务: MINUTE, HOURLY,
rem                                    DAILY, WEEKLY, MONTHLY, ONCE,
rem                                    ONSTART, ONLOGON, ONIDLE。
rem 
rem     /MO          modifier          改进计划类型以允许
rem                                    更好地控制计划重复
rem                                    周期。有效值列于
rem                                    下面"Modifiers" 部分中。
rem 
rem     /D           days              指定该周内运行任务的
rem                                    日期。有效值: MON, TUE,
rem                                    WED, THU, FRI, SAT, SUN
rem                                   和对 MONTHLY 计划的 1 - 31
rem                                    (某月中的日期)。
rem 
rem     /M           months            指定一年内的某月。
rem                                    默认是该月的第一天。
rem                                    有效值: JAN, FEB, MAR,
rem                                    APR, MAY, JUN, JUL, AUG, SEP, OCT,
rem                                    NOV, DEC。
rem 
rem     /I           idletime          指定运行一个已计划的
rem                                    ONIDLE 任务之前要等待
rem                                    的空闲时间。
rem                                    有效值范围: 1 到 999 分钟。
rem 
rem     /TN          taskname          指定唯一识别这个计划
rem                                    任务的名称。
rem 
rem     /TR          taskrun           指定这个计划的任务
rem                                    运行的程序的路径和
rem                                    文件名。
rem                                    示例: C:\windows\system32\calc.exe
rem 
rem     /ST          starttime         指定运行这个任务的时间。
rem                                    时间格式是 HH:MM:SS (24
rem                                    小时时间) 例如，14:30:00
rem                                    是下午 2:30 PM。
rem 
rem     /SD          startdate         指定第一次运行这个
rem                                    任务的日期。格式为
rem                                    "yyyy/mm/dd"。
rem 
rem     /ED          enddate           指定上次运行这个
rem                                    任务的日期。格式为
rem                                    "yyyy/mm/dd"。
rem 
rem 
rem 修改者: 按计划类型的 /MO 开关的有效值:
rem     MINUTE:  1 到 1439 分钟。
rem     HOURLY:  1 到 23 小时。
rem     DAILY:   1 到 365 天。
rem     WEEKLY:  1 到 52 周。
rem     ONCE:    无修改者。
rem     ONSTART: 无修改者。
rem     ONLOGON: 无修改者。
rem     ONIDLE:  无修改者。
rem     MONTHLY: 1 到 12，或
rem              FIRST, SECOND, THIRD, FOURTH, LAST, LASTDAY。
rem 
rem 示例:
rem     SCHTASKS /Create /S system /U user /P password /RU runasuser
rem              /RP runaspassword /SC HOURLY /TN rtest1 /TR logmanagement.exe的全路径
rem     SCHTASKS /Create /SC MONTHLY /MO first /D SUN /TN game
rem              /TR logmanagement.exe的全路径
rem     SCHTASKS /Create /S system /U user /P password /RU runasuser
rem              /RP runaspassword /SC WEEKLY /TN test1 /TR logmanagement.exe的全路径
rem     SCHTASKS /Create /S system /U domain\user /P password /SC MINUTE
rem              /MO 5 /TN test2 /TR logmanagement.exe的全路径
rem              /ST 18:30:00 /RU runasuser /RP *
rem     SCHTASKS /Create /SC MONTHLY /MO first /D SUN /TN cell
rem              /TR logmanagement.exe的全路径 /RU runasuser

SCHTASKS /Create /SC MONTHLY /MO first /D SUN /TN logmgr /TR logmanagement.exe的全路径




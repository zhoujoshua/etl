@echo off 
rem SCHTASKS /Create [/S system [/U username [/P password]]]
rem     [/RU username [/RP password]] /SC schedule [/MO modifier] [/D day]
rem     [/I idletime] /TN taskname /TR taskrun [/ST starttime] [/M months]
rem     [/SD startdate] [/ED enddate]
rem 
rem ����:
rem     ���ù���Ա���ڱ��ػ�Զ��ϵͳ��
rem     �����ƻ�����
rem 
rem �����б�:
rem     /S           system            ָ��Ҫ���ӵ���Զ��
rem                                    ϵͳ�����ʡ�����
rem                                    ϵͳ������Ĭ���Ǳ���
rem                                    ϵͳ��
rem 
rem     /U           username          ָ������ִ�е�
rem                                    �û������ġ�
rem 
rem     /P           password          ָ�������û�������
rem                                    �����롣
rem 
rem     /RU          username          ָ���������е��û�
rem                                    �ʻ� (�û�������)��
rem                                    ����ϵͳ�ʻ�����Чֵ�� ""��
rem                                    "NT AUTHORITY\SYSTEM" ��
rem                                    "SYSTEM"��
rem 
rem     /RP          password          ָ���û����롣
rem                                    Ҫ��ʾ�������룬ֵ
rem                                    ������ "*" ��û�С�
rem                                    ���뽫�����ϵͳ�ʻ�
rem                                    �����á�
rem 
rem     /SC          schedule          ָ���ƻ�Ƶ�ʡ�
rem                                    ��Ч�ƻ�����: MINUTE, HOURLY,
rem                                    DAILY, WEEKLY, MONTHLY, ONCE,
rem                                    ONSTART, ONLOGON, ONIDLE��
rem 
rem     /MO          modifier          �Ľ��ƻ�����������
rem                                    ���õؿ��Ƽƻ��ظ�
rem                                    ���ڡ���Чֵ����
rem                                    ����"Modifiers" �����С�
rem 
rem     /D           days              ָ�����������������
rem                                    ���ڡ���Чֵ: MON, TUE,
rem                                    WED, THU, FRI, SAT, SUN
rem                                   �Ͷ� MONTHLY �ƻ��� 1 - 31
rem                                    (ĳ���е�����)��
rem 
rem     /M           months            ָ��һ���ڵ�ĳ�¡�
rem                                    Ĭ���Ǹ��µĵ�һ�졣
rem                                    ��Чֵ: JAN, FEB, MAR,
rem                                    APR, MAY, JUN, JUL, AUG, SEP, OCT,
rem                                    NOV, DEC��
rem 
rem     /I           idletime          ָ������һ���Ѽƻ���
rem                                    ONIDLE ����֮ǰҪ�ȴ�
rem                                    �Ŀ���ʱ�䡣
rem                                    ��Чֵ��Χ: 1 �� 999 ���ӡ�
rem 
rem     /TN          taskname          ָ��Ψһʶ������ƻ�
rem                                    ��������ơ�
rem 
rem     /TR          taskrun           ָ������ƻ�������
rem                                    ���еĳ����·����
rem                                    �ļ�����
rem                                    ʾ��: C:\windows\system32\calc.exe
rem 
rem     /ST          starttime         ָ��������������ʱ�䡣
rem                                    ʱ���ʽ�� HH:MM:SS (24
rem                                    Сʱʱ��) ���磬14:30:00
rem                                    ������ 2:30 PM��
rem 
rem     /SD          startdate         ָ����һ���������
rem                                    ��������ڡ���ʽΪ
rem                                    "yyyy/mm/dd"��
rem 
rem     /ED          enddate           ָ���ϴ��������
rem                                    ��������ڡ���ʽΪ
rem                                    "yyyy/mm/dd"��
rem 
rem 
rem �޸���: ���ƻ����͵� /MO ���ص���Чֵ:
rem     MINUTE:  1 �� 1439 ���ӡ�
rem     HOURLY:  1 �� 23 Сʱ��
rem     DAILY:   1 �� 365 �졣
rem     WEEKLY:  1 �� 52 �ܡ�
rem     ONCE:    ���޸��ߡ�
rem     ONSTART: ���޸��ߡ�
rem     ONLOGON: ���޸��ߡ�
rem     ONIDLE:  ���޸��ߡ�
rem     MONTHLY: 1 �� 12����
rem              FIRST, SECOND, THIRD, FOURTH, LAST, LASTDAY��
rem 
rem ʾ��:
rem     SCHTASKS /Create /S system /U user /P password /RU runasuser
rem              /RP runaspassword /SC HOURLY /TN rtest1 /TR logmanagement.exe��ȫ·��
rem     SCHTASKS /Create /SC MONTHLY /MO first /D SUN /TN game
rem              /TR logmanagement.exe��ȫ·��
rem     SCHTASKS /Create /S system /U user /P password /RU runasuser
rem              /RP runaspassword /SC WEEKLY /TN test1 /TR logmanagement.exe��ȫ·��
rem     SCHTASKS /Create /S system /U domain\user /P password /SC MINUTE
rem              /MO 5 /TN test2 /TR logmanagement.exe��ȫ·��
rem              /ST 18:30:00 /RU runasuser /RP *
rem     SCHTASKS /Create /SC MONTHLY /MO first /D SUN /TN cell
rem              /TR logmanagement.exe��ȫ·�� /RU runasuser

SCHTASKS /Create /SC MONTHLY /MO first /D SUN /TN logmgr /TR logmanagement.exe��ȫ·��




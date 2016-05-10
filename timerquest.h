#ifndef TIMERQUEST_H
#define TIMERQUEST_H

#include<map>
#include<set>

typedef void (*FUNC)();

class TimerQuest;
class TimerQuestItem
{
public:
	TimerQuestItem(unsigned int id, TimerQuest* agent, FUNC quest_func);
	~TimerQuestItem();
	/*
		ִ�ж�ӦQuestһ��
		@Return Ϊtrue��ʾ��TimerQuest���ڸô����н�����ɾ��
		@author deadline
		@create 3/2/2012
	*/
	bool Run();

public:
	unsigned int m_id;

	FUNC m_timer_func;  // �����Ӧ�ĺ���

	bool m_is_always_run;
	double m_run_period;
	double m_next_run_time;
	unsigned int m_run_count;
	TimerQuest* m_parent;
};

class TimerQuest
{
public:
	TimerQuest();
	~TimerQuest();

	void Update(double now_time,double elapse_time); // ÿ֡���õ�update
	void Stop();

	bool isQuestAlive(unsigned int id);

	/*
		��ʱִ��һ������
		@para1 quest_func �����Ӧ�Ľű����� in
		@para2 delay_time ��ʱʱ�� in
		@para3 is_outter_keep �Ƿ�������Լ�ά��������������� in
		@Return �����������ڳ��ж���(��is_outter_keepΪtrueʱ��ֵ��nil)
		@author deadline
		@create 3/2/2012
	*/
	unsigned int AddDelayQuest(
		FUNC quest_func,
		double delay_time);

	/*
		������ִ��һ������
		@para1 quest_func �����Ӧ�Ľű����� in
		@para2 period ִ������ in
		@para3 last_time �������ʱ��(Ϊ����ʾ������������) in
		@para4 is_outter_keep �Ƿ�������Լ�ά��������������� in
		@Return �����������ڳ��ж���(��is_outter_keepΪtrueʱ��ֵ��nil)
		@author deadline
		@create 3/2/2012
	*/
	unsigned int AddPeriodQuest(
		FUNC quest_func,
			double period,
			double last_time)
	{

	}

	/*
		������ִ������ָ������
		@para1 quest_func �����Ӧ�Ľű����� in
		@para2 period ִ������ in
		@para3 run_count ����ִ�еĴ��� in
		@para4 is_outter_keep �Ƿ�������Լ�ά��������������� in
		@Return �����������ڳ��ж���(��is_outter_keepΪtrueʱ��ֵ��nil)
		@author deadline
		@create 3/2/2012
	*/
	unsigned int AddRunTimesQuest(
		FUNC quest_func,
		double period,
		int run_count)
	{

	}

	void CancelQuest(unsigned int quest_id);

protected:
	typedef std::multimap<double, TimerQuestItem*> TimerQuestItemMap;
	TimerQuestItemMap m_timer_quest_map;
	unsigned int m_id_count;
	double m_now_time;

	typedef std::set<unsigned int> KeepIdSet;
	KeepIdSet m_keep_id_set;
};



#endif
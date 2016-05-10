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
		执行对应Quest一次
		@Return 为true表示该TimerQuest将在该次运行结束后被删除
		@author deadline
		@create 3/2/2012
	*/
	bool Run();

public:
	unsigned int m_id;

	FUNC m_timer_func;  // 任务对应的函数

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

	void Update(double now_time,double elapse_time); // 每帧调用的update
	void Stop();

	bool isQuestAlive(unsigned int id);

	/*
		延时执行一个任务
		@para1 quest_func 任务对应的脚本函数 in
		@para2 delay_time 延时时间 in
		@para3 is_outter_keep 是否由外界自己维护任务的生命周期 in
		@Return 返回生命周期持有对象(仅is_outter_keep为true时该值非nil)
		@author deadline
		@create 3/2/2012
	*/
	unsigned int AddDelayQuest(
		FUNC quest_func,
		double delay_time);

	/*
		周期性执行一个任务
		@para1 quest_func 任务对应的脚本函数 in
		@para2 period 执行周期 in
		@para3 last_time 任务持续时间(为负表示任务永不过期) in
		@para4 is_outter_keep 是否由外界自己维护任务的生命周期 in
		@Return 返回生命周期持有对象(仅is_outter_keep为true时该值非nil)
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
		周期性执行任务指定次数
		@para1 quest_func 任务对应的脚本函数 in
		@para2 period 执行周期 in
		@para3 run_count 任务执行的次数 in
		@para4 is_outter_keep 是否由外界自己维护任务的生命周期 in
		@Return 返回生命周期持有对象(仅is_outter_keep为true时该值非nil)
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
#include "timerquest.h"
#include<vector>
TimerQuestItem::TimerQuestItem(unsigned int id, TimerQuest* agent, FUNC quest_func)
	:m_id(id)
	,m_parent(agent)
	,m_timer_func(quest_func)
{

}

TimerQuestItem::~TimerQuestItem()
{
	m_timer_func = 0;
}

bool TimerQuestItem::Run()
{
	/// 由外界自主控制生命周期的任务
	if(!m_parent->isQuestAlive(m_id))
	{
		// 外界已经不再执有该任务，删除该任务
		return true;
	}

	bool need_del = false;

	m_timer_func();  /// 调用函数

	return true;
}

TimerQuest::TimerQuest()
	:m_id_count(0)
	,m_now_time(0.0)
{

}

TimerQuest::~TimerQuest()
{
	Stop();
}

void TimerQuest::Stop()
{
	for(TimerQuestItemMap::iterator ihandle = m_timer_quest_map.begin(); ihandle != m_timer_quest_map.end(); ihandle++)
	{
		delete ihandle->second;
	}
	m_timer_quest_map.clear();
}

void TimerQuest::Update(double now_time,double elapse_time)
{
	m_now_time = now_time;
	TimerQuestItemMap::iterator iquest = m_timer_quest_map.upper_bound(now_time);

	typedef std::vector<TimerQuestItem*> TimerQuestItemVector;
	TimerQuestItemVector	tmp_quest_array;
	if(iquest == m_timer_quest_map.begin())
	{
		return;
	}

	for(TimerQuestItemMap::iterator ihandle = m_timer_quest_map.begin(); ihandle != iquest; ihandle ++)
	{
		tmp_quest_array.push_back(ihandle->second);
	}

	m_timer_quest_map.erase(m_timer_quest_map.begin(),iquest);

	for(size_t i = 0;  i < tmp_quest_array.size(); i++)
	{
		TimerQuestItem* quest = tmp_quest_array[i];
		bool need_del = quest->Run();
		if(!need_del)
		{
			m_timer_quest_map.insert(std::make_pair(quest->m_next_run_time,quest));
		}
		else
		{
			CancelQuest(quest->m_id);
			delete quest;
		}
	}
}

bool TimerQuest::isQuestAlive(unsigned int id)
{
	if(m_keep_id_set.find(id) != m_keep_id_set.end())
	{
		return true;
	}
	else
	{
		return false;
	}
}

void TimerQuest::CancelQuest(unsigned int quest_id)
{
	m_keep_id_set.erase(quest_id);
}

unsigned int TimerQuest::AddDelayQuest(FUNC quest_func,
	double delay_time)
{
	/// 延迟执行任务
	TimerQuestItem * quest = new TimerQuestItem(++m_id_count, this, quest_func);

	quest->m_is_always_run = false;

	quest->m_next_run_time = m_now_time + delay_time;

	quest->m_run_period = 10.0;
	
	m_timer_quest_map.insert(std::make_pair(quest->m_next_run_time,quest));
	m_keep_id_set.insert(quest->m_id);
	return quest->m_id;
}


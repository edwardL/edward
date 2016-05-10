#include<list>
#include<vector>

enum GAME_EVENT_ID;
struct EVENT;
typedef VOID (__stdcall* FUNC_EVENT_HANDLE)(const EVENT* pEvent, UINT dwOwnerData);

struct EVENT_DEFINE
{
	std::list<std::pair<FUNC_EVENT_HANDLE,UINT> > REGISTER_STRUCT;
	GAME_EVENT_ID idEvent;
};

struct EVENT
{
	EVENT_DEFINE* pEventDef;
	std::vector< std::string> vArg;
	bool operator == (const EVENT& other)
	{
		if(other.pEventDef != pEventDef) return false;
		if(other.vArg.size() != vArg.size()) return false;
	}
};

class tEventSystem
{
public:
	virtual VOID PushEvent(GAME_EVENT_ID id) = 0;
	virtual VOID PushEvent(GAME_EVENT_ID id, INT iArg0) =0;
};
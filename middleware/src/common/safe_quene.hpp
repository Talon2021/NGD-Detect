#ifndef SAFE_QUENE_
#define SAFE_QUENE_

#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
using  uint32 = unsigned int;
using  int32 = int;


template<typename tpType>
class SafeQueuee
{
    	static const uint32 kDefaultMaxsize = 4294967290U; //容量上限
public:
    	explicit SafeQueuee(bool pb_block = true, uint32 pn_maxsize = kDefaultMaxsize)
    		: mb_block(pb_block)
    		, mn_maxsize(pn_maxsize > 0 ? pn_maxsize : kDefaultMaxsize)
    		, mc_queue()
		, mb_push_waiting(0)
		, mb_pop_waiting(0) { }
	~SafeQueuee() { }
	bool push(const tpType & po_val)
	{
		{
			std::lock_guard<std::mutex> lock(mo_mutex);
			while ((mc_queue.size() >= mn_maxsize) && mb_block)
			{
				++mb_push_waiting;
				mo_cond_push.wait(mo_mutex);
				--mb_push_waiting;
			}
			if ((mc_queue.size() >= mn_maxsize) && (!mb_block))
			{
				return false;
			}
			mc_queue.push(po_val);
		} //锁已释放
		if (mb_pop_waiting > 0) { mo_cond_pop.notify_one(); }
		return true;
	}
	bool pop(tpType & po_val)
	{
		{
			std::lock_guard<std::mutex> lock(mo_mutex);
			while (mc_queue.empty() && mb_block)
			{
				++mb_pop_waiting;
				mo_cond_pop.wait(mo_mutex);
				--mb_pop_waiting;
			}
			if (mc_queue.empty() && (!mb_block))
			{
				return false;
			}			
			po_val = std::move(mc_queue.front());
			mc_queue.pop();
		} //锁已释放
 
		if (mb_push_waiting > 0) { mo_cond_push.notify_one(); }		
		return true;
	}
	bool pop()
	{
		{
			std::lock_guard<std::mutex> lock(mo_mutex);
			while (mc_queue.empty() && mb_block)
			{
				++mb_pop_waiting;
				mo_cond_pop.wait(mo_mutex);
				--mb_pop_waiting;
			}
			if (mc_queue.empty() && (!mb_block))
			{
				return false;
			}
			mc_queue.pop();
		}
		if (mb_push_waiting > 0) { mo_cond_push.notify_one(); }
		return true;
	}
	bool front(tpType & po_val)
	{
		std::lock_guard<std::mutex> lock(mo_mutex);
		if (mc_queue.empty()) {	return false; }
		po_val = mc_queue.front();
		return true;
	}
	bool empty()
	{
		std::lock_guard<std::mutex> lock(mo_mutex);
		return mc_queue.empty();
	}
	bool full()
	{
		std::lock_guard<std::mutex> lock(mo_mutex);
		return (mc_queue.size() >= mn_maxsize);
	}
	uint32 size()
	{
		std::lock_guard<std::mutex> lock(mo_mutex);
		return (uint32)mc_queue.size();
	}
	void clear()
	{
		{
			std::lock_guard<std::mutex> lock(mo_mutex);
			while (!mc_queue.empty()) {	mc_queue.pop();	}
		}
		mo_cond_push.notify_all();
	}
	void set_block(bool pb_block)
	{
		{
			std::lock_guard<std::mutex> lock(mo_mutex);
			if (mb_block == pb_block) {	return;	}
			mb_block = pb_block;
		}
		mo_cond_pop.notify_all();
		mo_cond_push.notify_all();
	}
	bool set_maxsize(uint32 pn_maxsize)
	{
		std::lock_guard<std::mutex> lock(mo_mutex);
		if ((pn_maxsize < mc_queue.size()) || (pn_maxsize <= 0))
		{
			return false;
		}
		mn_maxsize = pn_maxsize;
		return true;
	}
protected:
	//不可拷贝不可移动
	SafeQueuee(const SafeQueuee&) = delete;
	SafeQueuee(SafeQueuee&&) = delete;
	SafeQueuee& operator=(const SafeQueuee&) = delete;
	SafeQueuee& operator=(SafeQueuee&&) = delete;
protected:
	bool mb_block; //是否阻塞标记, true阻塞等待, false立即返回
	uint32 mn_maxsize; //大于等于上限将会加入失败或挂起等待
	std::queue< tpType > mc_queue;
	std::mutex mo_mutex;
	std::condition_variable_any mo_cond_push;
	std::condition_variable_any mo_cond_pop;
	std::atomic<int32> mb_push_waiting;
	std::atomic<int32> mb_pop_waiting;
};

#endif
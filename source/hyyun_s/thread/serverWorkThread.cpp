/*
 * epollWorkThread.h
 *
 *  Created on: Mar 26, 2014
 *      Author: root
 */

#include "../preheader.h"
#include "serverWorkThread.h"

namespace hyyun
{
	ServerWorkThread::ServerWorkThread(YunSelector& selector, s32& timeout)
			: _selector(selector)
			, _timeout(timeout)
			, pEvent(NULL)
			, pInfo(NULL) { }

	void ServerWorkThread::exit() { _selector.remove(NULL, NULL); _timeout = -1; Thread::exit(); }

	bool ServerWorkThread::onInvoke(void* lpParam)
	{
		try
		{
            arrHandler.clear();

			u32 i;
			u32 len;
			// 获得下一个句柄.
			_selector.select(&arrHandler, _timeout);

			i = 0;
			len = arrHandler.size();
			// 消息.
			for (; i < len; i++)
			{
				pEvent = arrHandler[i].pEvent;
				pInfo = arrHandler[i].pInfo;
				if (pEvent)
				{
					pEvent->finishAsync(pInfo);
				}
			} // for

		}
		catch(std::exception& ex)
		{
			trace_all(_T("ERROR break in EpollWorkItem %s!"), ex.what());
		}
		catch(...)
		{
			trace_all(_T("ERROR break in MainWorkItem!"));
		}
		return false;
	}
}

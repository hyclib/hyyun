/*
 * epollWorkThread.h
 *
 *  Created on: Mar 27, 2014
 *      Author: root
 */

#ifndef _BP_SERVERWORKTHREAD_H_201403271743_
#define _BP_SERVERWORKTHREAD_H_201403271743_


namespace hyyun
{
	class HYYUN_API ServerWorkThread : public Thread
	{
	public:
		YunSelector&	_selector;
		s32			    _timeout;

		SelectorEvent*              pEvent;
		AsyncInfo*                  pInfo;
		Array<SelectorEventInfo>    arrHandler;

	public:
		ServerWorkThread(YunSelector& selector, s32& timeout);

		virtual void	exit();

		virtual bool    onInvoke(void* lpParam);
	};
}



#endif /* _BP_SERVERWORKTHREAD_H_201403271743_ */

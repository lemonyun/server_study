#include "pch.h"
#include "JobQueue.h"
/*-----------------
	JobQueue
------------------*/

void JobQueue::Push(JobRef&& job)
{
	const int32 prevCount = _jobCount.fetch_add(1); // count 증가 후
	_jobs.Push(job); // 내부적으로 WRITE_LOCK // job을 push (윗줄과 순서가 바뀌면 안됨)

	// 첫번째 Job을 넣은 쓰레드가 실행까지 담당
	if (prevCount == 0)
	{
		Execute();
	}
}

// 1) 일감이 너무 몰리면?
// 2) DoAync 타고 타고 가서 절대 끝나지 않는 상황

void JobQueue::Execute()
{
	while (true)
	{
		Vector<JobRef> jobs;
		_jobs.PopAll(OUT jobs);

		const int32 jobCount = static_cast<int32>(jobs.size());
		for (int32 i = 0; i < jobCount; i++)
			jobs[i]->Execute(); // job을 실행한 뒤
		
		// 남은 일감이 0개라면 종료
		if (_jobCount.fetch_sub(jobCount) == jobCount) // count를 감소 
		{
			return;
		}
	}
}

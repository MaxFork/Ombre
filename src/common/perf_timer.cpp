// Copyright (c) 2018, Ombre Cryptocurrency Project
// Copyright (c) 2018, Ryo Currency Project
// Portions copyright (c) 2014-2018, The Monero Project
//
// Portions of this file are available under BSD-3 license. Please see ORIGINAL-LICENSE for details
// All rights reserved.
//
// Ombre changes to this code are in public domain. Please note, other licences may apply to the file.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
// THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
// STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
// THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include "perf_timer.h"
#include "misc_os_dependent.h"
#include <vector>

#undef MONERO_DEFAULT_LOG_CATEGORY
#define MONERO_DEFAULT_LOG_CATEGORY "perf"

namespace tools
{
uint64_t get_tick_count()
{
#if defined(__x86_64__)
	uint32_t hi, lo;
	__asm__ volatile("rdtsc"
					 : "=a"(lo), "=d"(hi));
	return (((uint64_t)hi) << 32) | (uint64_t)lo;
#else
	return epee::misc_utils::get_ns_count();
#endif
}

#ifdef __x86_64__
uint64_t get_ticks_per_ns()
{
	uint64_t t0 = epee::misc_utils::get_ns_count(), t1;
	uint64_t r0 = get_tick_count();

	while(1)
	{
		t1 = epee::misc_utils::get_ns_count();
		if(t1 - t0 > 1 * 1000000000)
			break; // work one second
	}

	uint64_t r1 = get_tick_count();
	uint64_t tpns256 = 256 * (r1 - r0) / (t1 - t0);
	return tpns256 ? tpns256 : 1;
}
#endif

#ifdef __x86_64__
uint64_t ticks_per_ns = get_ticks_per_ns();
#endif

uint64_t ticks_to_ns(uint64_t ticks)
{
#if defined(__x86_64__)
	return 256 * ticks / ticks_per_ns;
#else
	return ticks;
#endif
}
}

namespace tools
{

el::Level performance_timer_log_level = el::Level::Debug;

static __thread std::vector<LoggingPerformanceTimer *> *performance_timers = NULL;

void set_performance_timer_log_level(el::Level level)
{
	if(level != el::Level::Debug && level != el::Level::Trace && level != el::Level::Info && level != el::Level::Warning && level != el::Level::Error && level != el::Level::Fatal)
	{
		MERROR("Wrong log level: " << el::LevelHelper::convertToString(level) << ", using Debug");
		level = el::Level::Debug;
	}
	performance_timer_log_level = level;
}

PerformanceTimer::PerformanceTimer(bool paused) : started(true), paused(paused)
{
	if(paused)
		ticks = 0;
	else
		ticks = get_tick_count();
}

LoggingPerformanceTimer::LoggingPerformanceTimer(const std::string &s, const std::string &cat, uint64_t unit, el::Level l) : PerformanceTimer(), name(s), cat(cat), unit(unit), level(l)
{
	if(!performance_timers)
	{
		MCLOG(level, cat.c_str(), "PERF             ----------");
		performance_timers = new std::vector<LoggingPerformanceTimer *>();
		performance_timers->reserve(16); // how deep before realloc
	}
	else
	{
		LoggingPerformanceTimer *pt = performance_timers->back();
		if(!pt->started && !pt->paused)
		{
			size_t size = 0;
			for(const auto *tmp : *performance_timers)
				if(!tmp->paused)
					++size;
			MCLOG(pt->level, cat.c_str(), "PERF           " << std::string((size - 1) * 2, ' ') << "  " << pt->name);
			pt->started = true;
		}
	}
	performance_timers->push_back(this);
}

PerformanceTimer::~PerformanceTimer()
{
	if(!paused)
		ticks = get_tick_count() - ticks;
}

LoggingPerformanceTimer::~LoggingPerformanceTimer()
{
	pause();
	performance_timers->pop_back();
	char s[12];
	snprintf(s, sizeof(s), "%8llu  ", (unsigned long long)(ticks_to_ns(ticks) / (1000000000 / unit)));
	size_t size = 0;
	for(const auto *tmp : *performance_timers)
		if(!tmp->paused || tmp == this)
			++size;
	MCLOG(level, cat.c_str(), "PERF " << s << std::string(size * 2, ' ') << "  " << name);
	if(performance_timers->empty())
	{
		delete performance_timers;
		performance_timers = NULL;
	}
}

void PerformanceTimer::pause()
{
	if(paused)
		return;
	ticks = get_tick_count() - ticks;
	paused = true;
}

void PerformanceTimer::resume()
{
	if(!paused)
		return;
	ticks = get_tick_count() - ticks;
	paused = false;
}
}

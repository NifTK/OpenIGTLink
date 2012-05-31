#include "ptpTimer.h"


namespace ptp
{
  //Definition of static member variable
  int Timer::elapsed[10];
 


/* catch_alarm:
 * Simple function to be called by the operating
 * system on a periodic basis, update the
 * private timer array table and return
 */
#if defined(__WINDOWS__) || defined(_WIN32)
VOID CALLBACK Timer::catch_alarm(PVOID lpParameter, BOOLEAN TimerOrWaitFired)
#else
void Timer::catch_alarm(int sig)
#endif
{
  int i;

  for (i=0; i<MAX_PTP_PORTS; i++)
  {
    elapsed[i] += TIMER_INTERVAL;
  }
  // NOTE: If you put in a debug print here, it will interleave with other messages
  // from the normal execution task.  So if you need to put one in, put it
  // in to verify it, but remove it once you verify timeout is occurring OK.
#ifdef TEMPORARY_DEBUG_TIMER_TICK
#ifdef PTPD_DBG
  if (debugLevel >= 1) 
  {
    DBGV("\n(***tick %u***)\n",elapsed[0]);
    fflush(NULL);
  }
#endif
#endif

  
  /*  fprintf(stderr, "tick: %d, %d\n",max_ticks,license_tick_counter); */

#ifdef LIMIT_RUNTIME
 /* Check Limit run timer */
 if (++license_tick_counter > max_ticks)
 {
    fprintf(stderr, "\nptpv2d: Evaluation license maximum run time reached, terminating!\n"); 
    raise(SIGTERM); // Time up, send a terminate signal to the ptpv2d task
 }
 /* End run time limit timer*/
#endif

}

/** Function to initialize the global timer for ptpv2d
 * timer functions
 */
void Timer::initTimer(Integer32  seconds, UInteger32 microseconds)
{
#if defined(__WINDOWS__) || defined(_WIN32)
  BOOL success;
#else
  struct itimerval itimer;
  int i;  
#endif

  //DBG("initTimer: %d seconds, %u microseconds\n", seconds, microseconds);
#if defined(__WINDOWS__) || defined(_WIN32)
    success = CreateTimerQueueTimer
       (
        &TimerQueue,            // Handle for timer
        NULL,                   // NULL for default timer queue
        catch_alarm,      // Callback function (NOTE: must be fast)
        NULL,                   // Pointer to data for callback function
        0,                      // Time (ms) before timer signaled for the first time
        ( (seconds*1000)        // Interval time in milliseconds
         +(microseconds/1000)
        ),
        WT_EXECUTEINIOTHREAD
        //WT_EXECUTEINTIMERTHREAD // Run in timer thread (callback function must be quick
       );
    //if (!success) DBG("Failed to created windows timer\n");
#else
  //
  // Not Windows code (e.g. Linux)
  //
  // Set Alarm clock signal to ignore
  signal(SIGALRM, SIG_IGN);

  //
  // Clear elapsed time for all ports
  //
  for (i=0; i<MAX_PTP_PORTS; i++)
  {
      elapsed[i] = 0;
  }

#ifdef LIMIT_RUNTIME
  // Adjust run time limit timer if timer less than one second per tick
  if (seconds == 0)
  {
    max_ticks *= (1000000/microseconds);
  }
#endif

  // Set interval timer values  
  itimer.it_value.tv_sec  = itimer.it_interval.tv_sec  = seconds;
  itimer.it_value.tv_usec = itimer.it_interval.tv_usec = microseconds;
  
  // Set Alarm clock signal to catch_alarm function above
  signal(SIGALRM, catch_alarm);

  // Set and start interval timer
  setitimer(ITIMER_REAL,  // Use real time clock (do not use time of day)
            &itimer,      // Pointer to new itimerval structure 
            0             // Pointer to old structure (none in this case)
           );  
#endif
}

void Timer::timerUpdate(IntervalTimer *itimer, int port_id)
{
  int i, delta;
  
  i = port_id -1;
  delta   = elapsed[i];
  elapsed[i] = 0;
  
  if(delta <= 0)
    return;
  
  for(i = 0; i < TIMER_ARRAY_SIZE; ++i)
  {
    if(itimer[i].interval > 0 && (itimer[i].left -= delta) <= 0)
    {
      itimer[i].left   = itimer[i].interval;
      itimer[i].expire = true;
      //DBGV("timerUpdate: timer index %u expired (interval=%d delta=%d)\n",i,itimer[i].interval,delta);
    }
  }
}

void Timer::timerStop(UInteger16 index, IntervalTimer *itimer)
{
  if(index >= TIMER_ARRAY_SIZE)
    return;
  
  itimer[index].interval = 0;
  //DBGV("timerStop: timer index %u stopped\n", index);
}

void Timer::timerStart(UInteger16 index, UInteger16 interval, IntervalTimer *itimer)
{
  if(index >= TIMER_ARRAY_SIZE)
    return;
  
  itimer[index].expire   = false;
  itimer[index].left     = interval;
  itimer[index].interval = itimer[index].left;
  
  //DBGV("timerStart: set timer index %d to %d\n", index, interval);
}

Boolean Timer::timerExpired(UInteger16 index, IntervalTimer *itimer, int port_id)
{
  //DBGV("timerExpired: Checking index %d\n", index);
  timerUpdate(itimer, port_id);
  
  if(index >= TIMER_ARRAY_SIZE)
    return false;                // ERROR:index out of range, return false
  
  if(!itimer[index].expire)
    return false;                // Timer not expired, return FALSE
  
  //DBGV("timerExpired: timer index %d has expired\n", index);
  itimer[index].expire = false;  // Clear expired flag
  return true;                   // Return TRUE
}

}

// eof timer.c

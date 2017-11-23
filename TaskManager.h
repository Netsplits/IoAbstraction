#ifndef _TIMER_MANAGER_H_
#define _TIMER_MANAGER_H_

typedef void (*TimerFn)();
typedef void (*InterruptFn)(uint8_t);

#define TASK_IN_USE     0x8000
#define TASK_REPEATING  0x4000
#define TASK_MILLIS     0x2000
#define TASK_SECONDS    0x1000
#define TASK_MICROS     0x0000
#define TIMER_MASK      0x0fff

#define DEFAULT_TASK_SIZE 6

enum TimerUnit : byte {
	TIME_MICROS = 0, TIME_SECONDS = 1, TIME_MILLIS=2
};

class TimerTask {
private:
	uint16_t executionInfo;
	uint32_t scheduledAt;
	TimerFn callback;
public:
	TimerTask();
	bool isReady(unsigned long now);
	void initialise(uint16_t executionInfo, TimerFn execCallback);
	void execute();
	bool isInUse() { return (executionInfo & TASK_IN_USE) != 0; }
	bool isRepeating() { return (executionInfo & TASK_REPEATING) != 0; }
	void clear();
	int getTimerValue();
};

class TaskManager {
private:
	TimerTask *tasks;
	uint8_t numberOfSlots;
	InterruptFn interruptCallback;
	volatile uint8_t lastInterruptTrigger;
	volatile bool interrupted;
public:
	TaskManager(uint8_t taskSlots = DEFAULT_TASK_SIZE);

	uint8_t scheduleOnce(int millis, TimerFn timerFunction, TimerUnit timeUnit = TIME_MILLIS);
	uint8_t scheduleFixedRate(int millis, TimerFn timerFunction, TimerUnit timeUnit = TIME_MILLIS);
	void addInterrupt(uint8_t pin, uint8_t mode);
	void setInterruptCallback(InterruptFn handler);
	void cancelTask(uint8_t task);
	char* checkAvailableSlots(char* slotData);

	// this should be pretty much the only code in loop()
	void runLoop();

	static void markInterrupted(uint8_t interruptNo);
	static TaskManager* __taskMgrInstance;
private:
	int findFreeTask();
};

#endif

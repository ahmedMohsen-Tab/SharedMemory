
#include "SemaphoreGuard.hpp"

SemaphoreGuard::SemaphoreGuard(sem_t* sem)
    : m_sem(sem), m_locked(false)
{
    /** 
     * This locks the semaphore - no other processes can access the memory right now!
     * If the semaphore value is > 0 → decrement it and continue.
     * If the semaphore value is 0 → block until someone calls sem_post().
     */
    if (0 == sem_wait(m_sem)) { //< This blocks the process and puts it to sleep so it doesn't waste CPU cycles "staring" at the whiteboard.
        m_locked = true;
    }
}

SemaphoreGuard::~SemaphoreGuard() noexcept {
    // Unlocks the semaphore pointed to by sem by incrementing its value.
    // Waking Threads: If the semaphore value was 0 and threads were blocked waiting, one is woken up.
    if(m_locked) {
        if(0 != sem_post(m_sem)) { //< increments the semaphore
            perror("SemaphoreGuard: ");
        } 
    }
}

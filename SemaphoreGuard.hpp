#ifndef SemaphoreGuard_HPP_
#define SemaphoreGuard_HPP_


#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <semaphore.h>
#include <string.h>
#include <mutex>
#include <chrono>
#include <thread>
#include <cstring>
#include <iostream>
#include <memory>
#include <stdexcept>

/**
 * @brief RAII Semaphore code 
 * 
 */
class SemaphoreGuard {
    public:
        /**
         * @brief locks the semaphore - no other processes can access the memory right now! 
         *  
         * If the semaphore value is > 0 → decrement it and continue.
         * If the semaphore value is 0 → block until someone calls sem_post().
         * 
         * @param sem 
         */
        explicit SemaphoreGuard(sem_t* sem);

        /**
         * @brief Destroy the Semaphore Guard object
         * 
         */
        ~SemaphoreGuard() noexcept ;

        //< no Copying - This will break Synchronization somehow.
        SemaphoreGuard(const SemaphoreGuard&) = delete;
        SemaphoreGuard& operator=(const SemaphoreGuard&) = delete;

        /**
         * @brief 
         * 
         * @return true 
         * @return false 
         */
        [[nodiscard]] inline bool isLocked() const { return m_locked; }

        /**
         * @brief general method return the current error happen!!
         * 
         * @return std::string 
         */
        [[nodiscard]] inline std::string error() const { return std::string(strerror(errno)); }

    private:
        /**
         * @brief 
         * 
         */
        sem_t* m_sem;

        /**
         * @brief if the semaphore is locked or not !! 
         * 
         */
        bool m_locked;
};

#endif
#ifndef SharedMemory_HPP_
#define SharedMemory_HPP_

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
#include "atomic"
#include "SemaphoreGuard.hpp"


struct SharedMemoryConfiguration {
    /**
     * @brief shared memory name 
     * 
     */
    std::string shm_name = std::string();

    /**
     * @brief shared memory size 
     * 
     */
    size_t shm_size = 0;

    /**
     * @brief shared memory data 
     * 
     */
    void* shm_data = MAP_FAILED;

    /**
     * @brief Who own this shared memory 
     * 
     */
    bool shm_owner = false; 

    /**
     * @brief shared memory file descriptor set by 
     * 
     */
    int shm_fd = 0;
    
    /**
     * @brief Is Configuration is valid or not !! 
     * @note: 
     *  valid configuration does not mean the shared memory is ready to be used!!
     *  you must check is_initialized first
     * 
     * @return true 
     * @return false 
     */
    [[nodiscard]] inline bool isValid() const {
        return (!shm_name.empty() && (shm_size > 0));
    }
};

class SharedMemory {
    public:
        explicit SharedMemory(const SharedMemoryConfiguration& shm_configuration, const std::string& sem_name);
        ~SharedMemory() noexcept;

        SharedMemory(const SharedMemory&) = delete;
        SharedMemory& operator=(const SharedMemory&) = delete;

        /**
         * @brief check if the sharedMemory are ready to Read/Write ot not
         * Its critical function - do not read/write before check this 
         * 
         * @return true 
         * @return false 
         */
        [[nodiscard]] bool isInitialized() const noexcept;

        /**
         * @brief send data 
         *  NOTE:
         *      - In the shared memory we reserve the first (int 4 or 8 byte based on your machine) bytes to know the size of shared data here!! 
         * 
         * @param data 
         * @return true 
         * @return false 
         */
        [[nodiscard]] bool write(const std::string& data);

        /**
         * @brief read from shared memory
         *  The function read the size first then read the acutal data. 
         * @return std::string 
         */
        [[nodiscard]] std::string read();  

    private:
        /**
         * @brief 
         * 
         * @return true 
         * @return false 
         */
        [[nodiscard]] bool init() noexcept;
        
        /**
         * @brief 
         * 
         */
        inline void cleanUp() noexcept;

        /**
         * @brief Hold Shared memory configuration 
         * 
         */
        SharedMemoryConfiguration m_configuration;

        /**
         * @brief mutex synchronization 
         * 
         */
        mutable std::mutex m_mutex;
        mutable sem_t* m_semaphore; //< semaphore ptr 
        std::string m_sem_name;     //< semaphore name 


        /**
         * @brief is the shared memory ready to be used or not ?
         * In C++, std::atomic types are not copyable. 
         * Because an atomic operation is designed to be a single, thread-safe action.
         */
        std::atomic<bool> m_is_initialized;
};

#endif
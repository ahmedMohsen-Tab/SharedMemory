#include "SharedMemory.hpp"

SharedMemory::SharedMemory(const SharedMemoryConfiguration& shm_configuration, const std::string& sem_name)
    : m_configuration {shm_configuration},
      m_sem_name(sem_name),  
      m_is_initialized(false)
      
{    
    if(init()) {
        //< we should put any type of logs here
    }
}

SharedMemory::~SharedMemory() {
    cleanUp();
}

bool SharedMemory::init() noexcept {
    if(!m_configuration.isValid()) {
        return false; //< no need to continue if the configuration is not valid!!
    }

    /**
     * If the owner crashes, the old (possibly corrupted) shared memory segment still exists. 
     * When the owner restarts, it will open the old segment instead of creating a fresh one, 
     * because O_CREAT won't do anything if the file already exists.
     */
    if (m_configuration.shm_owner) {
        shm_unlink(m_configuration.shm_name.c_str());
        sem_unlink(m_sem_name.c_str());
    }

    m_configuration.shm_fd = shm_open(m_configuration.shm_name.c_str(),(m_configuration.shm_owner ? (O_CREAT | O_RDWR):(O_RDWR)),0666);

    if(-1 == m_configuration.shm_fd) {
        return false;
    }

    if(m_configuration.shm_owner) {
        if(-1 == ftruncate(m_configuration.shm_fd, m_configuration.shm_size)) {
            cleanUp();
            return false;
        }
    }

    m_configuration.shm_data = mmap(0, m_configuration.shm_size, (PROT_READ | PROT_WRITE), MAP_SHARED, m_configuration.shm_fd, 0);
    if (MAP_FAILED == m_configuration.shm_data) {
        cleanUp();
        return false;            
    }

    m_semaphore = sem_open(m_sem_name.c_str(), (m_configuration.shm_owner ? O_CREAT : 0), 0666, 1);
    if (SEM_FAILED == m_semaphore) {
        cleanUp();
        return false;
    }

    m_is_initialized.store(true);
    return true;
}

bool SharedMemory::isInitialized() const noexcept {
    return m_is_initialized;
}

bool SharedMemory::write(const std::string& data) {
    
    if(data.empty() || !m_is_initialized || ((data.size() + sizeof(int)) > m_configuration.shm_size)) {
        return false;
    }

    std::lock_guard<std::mutex> lock(m_mutex);
    SemaphoreGuard semLock(m_semaphore);
    {
        //< at offset 0 -> send dataSize value with size = sizeof(int)
        int sizeToWrite = data.size();
        std::memcpy(static_cast<char*>(m_configuration.shm_data), &sizeToWrite, sizeof(int));
        
        //< First we move size data to the shared memory then we send the actual data 
        //< Copy data from data into sgared memory !!
        std::copy(data.begin(), data.end(), static_cast<char*>(m_configuration.shm_data) + sizeof(int));
    }
    return true;
}

std::string SharedMemory::read() {
    if(!isInitialized()) {
        return std::string();
    }
    
    std::lock_guard<std::mutex> lock(m_mutex); //< For threads 
    SemaphoreGuard semLock(m_semaphore);       //< For Processes 
    
    if(!semLock.isLocked()) {
        //< an issue happen with the memory segment
        //< CleanUp and Connect agin to it!! <- let us wait for 1 msec->
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        cleanUp();
        init();
        return std::string();  
    }

    int dataSize = 0;
    std::memcpy(&dataSize, m_configuration.shm_data, sizeof(int));

    if(dataSize <= 0) {
        return std::string();
    }

    return std::string(static_cast<const char*>(m_configuration.shm_data) + sizeof(int), dataSize);
}


void SharedMemory::cleanUp() noexcept {
    if (MAP_FAILED != m_configuration.shm_data) {
        munmap(m_configuration.shm_data, m_configuration.shm_size);
        m_configuration.shm_data = MAP_FAILED;
    }   
    
    if (-1 != m_configuration.shm_fd) {
        close(m_configuration.shm_fd);
        m_configuration.shm_fd = -1;
    }

    if (SEM_FAILED != m_semaphore) {
        sem_close(m_semaphore);
        m_semaphore = SEM_FAILED;
    }

    m_is_initialized.store(false);

    if (m_configuration.shm_owner) {
        shm_unlink(m_configuration.shm_name.c_str());
        sem_unlink(m_sem_name.c_str());
    }
}
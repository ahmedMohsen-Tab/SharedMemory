#include <iostream>
#include <gtest/gtest.h>
#include "SharedMemory.hpp"

TEST(SharedMemoryTest, InitializedSharedMemory) {
    SharedMemoryConfiguration config{
        .shm_name = "/dummy_shared_memory",
        .shm_size = 5 * 1024 * 1024, //< 5 MB
        .shm_owner = true 
    };

    SharedMemory shared_memory(config, "/dummy_sem_name");
    EXPECT_EQ(shared_memory.isInitialized(), true);
}

TEST(SharedMemoryTest, ClientAttacth) {
    SharedMemoryConfiguration config{
        .shm_name = "/dummy_shared_memory",
        .shm_size = 5 * 1024 * 1024, //< 5 MB
        .shm_owner = true 
    };

    SharedMemory shared_memory(config, "/dummy_sem_name");
    ASSERT_TRUE(shared_memory.isInitialized());


    //< Client Configuration 
    SharedMemoryConfiguration clientConfig{
        .shm_name = "/dummy_shared_memory",
        .shm_size = 5 * 1024 * 1024, //< 5 MB
        .shm_owner = false
    };

    SharedMemory shared_memory_client(clientConfig, "/dummy_sem_name");
    ASSERT_TRUE(shared_memory_client.isInitialized());
} 


// //< If The Owner did not establish the memory first - the client should fail.
TEST(SharedMemoryTest, ClientFaileToConnectWithoutOwner) {
    //< Client Configuration 
    SharedMemoryConfiguration clientConfig{
        .shm_name = "/dummy_shared_memory",
        .shm_size = 5 * 1024 * 1024, //< 5 MB
        .shm_owner = false
    };

    SharedMemory shared_memory_client(clientConfig, "/dummy_sem_name");
    ASSERT_FALSE(shared_memory_client.isInitialized());
} 

TEST(SharedMemoryTest, ReadWriteTest) {
    SharedMemoryConfiguration ServerConfig{
        .shm_name  = "/dummy_shared_memory",
        .shm_size  = 5 * 1024 * 1024, //< 5 MB
        .shm_owner = true 
    };

    SharedMemory shared_memory(ServerConfig, "/dummy_sem_name");
    ASSERT_TRUE(shared_memory.isInitialized());


    //< Client Configuration 
    SharedMemoryConfiguration clientConfig{
        .shm_name = "/dummy_shared_memory",
        .shm_size = 5 * 1024 * 1024, //< 5 MB
        .shm_owner = false
    };

    SharedMemory shared_memory_client(clientConfig, "/dummy_sem_name");
    ASSERT_TRUE(shared_memory_client.isInitialized());


    //< send data from server 
    ASSERT_TRUE(shared_memory.write("Hello From Server"));
    EXPECT_STREQ(shared_memory_client.read().c_str(), std::string("Hello From Server").c_str());
}


TEST(SharedMemoryTest, WriteOverflowTest) {
    SharedMemoryConfiguration ServerConfig{
        .shm_name  = "/dummy_shared_memory",
        .shm_size  = 1 , //< 1 Byte 
        .shm_owner = true 
    };

    SharedMemory shared_memory(ServerConfig, "/dummy_sem_name");
    ASSERT_TRUE(shared_memory.isInitialized());


    //< Client Configuration 
    SharedMemoryConfiguration clientConfig{
        .shm_name = "/dummy_shared_memory",
        .shm_size = 1, //< 1 B
        .shm_owner = false
    };

    SharedMemory shared_memory_client(clientConfig, "/dummy_sem_name");
    ASSERT_TRUE(shared_memory_client.isInitialized());


    //< send data from server 
    ASSERT_FALSE(shared_memory.write("Hello From Server"));
    EXPECT_STREQ(shared_memory_client.read().c_str(), std::string().c_str());
}

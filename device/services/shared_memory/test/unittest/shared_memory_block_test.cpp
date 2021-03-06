/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <cstring>
#include <hwext/gtest-ext.h>
#include <hwext/gtest-tag.h>

#include "plugin_service_types.pb.h"
#include "share_memory_block.h"

using namespace testing::ext;

namespace {
constexpr size_t ARRAYSIZE = 1024;

class SharedMemoryBlockTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}

    void SetUp() {}
    void TearDown() {}
};

/**
 * @tc.name: share memory
 * @tc.desc: read lock.
 * @tc.type: FUNC
 */
HWTEST_F(SharedMemoryBlockTest, ReadLock, TestSize.Level1)
{
    ShareMemoryBlock shareMemoryBlock("testname", 4096);
    ASSERT_TRUE(shareMemoryBlock.Valid());

    ASSERT_TRUE(shareMemoryBlock.ReleaseBlock());
}

/**
 * @tc.name: share memory
 * @tc.desc: get name.
 * @tc.type: FUNC
 */
HWTEST_F(SharedMemoryBlockTest, GetName, TestSize.Level1)
{
    ShareMemoryBlock shareMemoryBlock("testname", 4096);
    ASSERT_TRUE(shareMemoryBlock.Valid());

    shareMemoryBlock.GetName();

    ASSERT_TRUE(shareMemoryBlock.ReleaseBlock());
}

/**
 * @tc.name: share memory
 * @tc.desc: get size.
 * @tc.type: FUNC
 */
HWTEST_F(SharedMemoryBlockTest, GetSize, TestSize.Level1)
{
    ShareMemoryBlock shareMemoryBlock("testname", 4096);
    ASSERT_TRUE(shareMemoryBlock.Valid());

    shareMemoryBlock.GetSize();

    ASSERT_TRUE(shareMemoryBlock.ReleaseBlock());
}

/**
 * @tc.name: share memory
 * @tc.desc: get file descriptor.
 * @tc.type: FUNC
 */
HWTEST_F(SharedMemoryBlockTest, GetfileDescriptor, TestSize.Level1)
{
    ShareMemoryBlock shareMemoryBlock("testname", 4096);
    ASSERT_TRUE(shareMemoryBlock.Valid());

    shareMemoryBlock.GetfileDescriptor();

    ASSERT_TRUE(shareMemoryBlock.ReleaseBlock());
}

/**
 * @tc.name: share memory
 * @tc.desc: Shared memory type test.
 * @tc.type: FUNC
 */
HWTEST_F(SharedMemoryBlockTest, DROP_NONE, TestSize.Level1)
{
    ShareMemoryBlock shareMemoryBlock("testname", 4096);
    ASSERT_TRUE(shareMemoryBlock.Valid());

    shareMemoryBlock.SetReusePolicy(ShareMemoryBlock::ReusePolicy::DROP_NONE);

    int8_t data[ARRAYSIZE];
    for (int i = 0; i < 5; i++) {
        *((uint32_t*)data) = i;
        shareMemoryBlock.PutRaw(data, ARRAYSIZE);
    }
    int8_t* p = shareMemoryBlock.GetFreeMemory(ARRAYSIZE);
    ASSERT_TRUE(p == nullptr);

    do {
        p = const_cast<int8_t*>(shareMemoryBlock.GetDataPoint());
    } while (shareMemoryBlock.Next() && shareMemoryBlock.GetDataSize() > 0);

    ASSERT_TRUE(shareMemoryBlock.ReleaseBlock());
}

/**
 * @tc.name: share memory
 * @tc.desc: Shared memory type test.
 * @tc.type: FUNC
 */
HWTEST_F(SharedMemoryBlockTest, DROP_OLD, TestSize.Level1)
{
    ShareMemoryBlock shareMemoryBlock("testname", 4096);
    ASSERT_TRUE(shareMemoryBlock.Valid());

    shareMemoryBlock.SetReusePolicy(ShareMemoryBlock::ReusePolicy::DROP_OLD);

    int8_t data[ARRAYSIZE];
    for (int i = 0; i < 5; i++) {
        *((uint32_t*)data) = i;
        shareMemoryBlock.PutRaw(data, ARRAYSIZE);
    }
    int8_t* p = shareMemoryBlock.GetFreeMemory(ARRAYSIZE);
    ASSERT_TRUE(p != nullptr);

    do {
        p = const_cast<int8_t*>(shareMemoryBlock.GetDataPoint());
    } while (shareMemoryBlock.Next() && shareMemoryBlock.GetDataSize() > 0);

    ASSERT_TRUE(shareMemoryBlock.ReleaseBlock());
}

/**
 * @tc.name: share memory
 * @tc.desc: put protobuf.
 * @tc.type: FUNC
 */
HWTEST_F(SharedMemoryBlockTest, PutMessage, TestSize.Level1)
{
    ShareMemoryBlock shareMemoryBlock("testname", 4096);
    ASSERT_TRUE(shareMemoryBlock.Valid());
    ASSERT_TRUE(shareMemoryBlock.GetDataSize() == 0);

    NotifyResultResponse response;
    response.set_status(123);
    ASSERT_TRUE(shareMemoryBlock.PutMessage(response));
    EXPECT_EQ(shareMemoryBlock.GetDataSize(), response.ByteSizeLong());
    response.ParseFromArray(shareMemoryBlock.GetDataPoint(), shareMemoryBlock.GetDataSize());
    ASSERT_TRUE(response.status() == 123);

    // ??????next???????????????????????????
    shareMemoryBlock.Next();
    NotifyResultResponse response2;
    response2.set_status(2345);
    ASSERT_TRUE(shareMemoryBlock.PutMessage(response2));
    EXPECT_EQ(shareMemoryBlock.GetDataSize(), response2.ByteSizeLong());
    response2.ParseFromArray(shareMemoryBlock.GetDataPoint(), shareMemoryBlock.GetDataSize());
    EXPECT_TRUE(response2.status() == 2345);

    // ??????next????????????message
    shareMemoryBlock.Next();
    NotifyResultRequest request;
    ASSERT_TRUE(shareMemoryBlock.PutMessage(request));
    EXPECT_EQ(shareMemoryBlock.GetDataSize(), request.ByteSizeLong());

    ASSERT_TRUE(shareMemoryBlock.ReleaseBlock());
}

/**
 * @tc.name: share memory
 * @tc.desc: Shared memory PutMessage abnormal test.
 * @tc.type: FUNC
 */
HWTEST_F(SharedMemoryBlockTest, PutMessageAbnormal, TestSize.Level1)
{
    ShareMemoryBlock shareMemoryBlock("testname", 4096);
    ASSERT_TRUE(shareMemoryBlock.Valid());
    ASSERT_TRUE(shareMemoryBlock.GetDataSize() == 0);

    NotifyResultResponse response;
    response.set_status(123);
    ASSERT_TRUE(shareMemoryBlock.PutMessage(response));
    EXPECT_EQ(shareMemoryBlock.GetDataSize(), response.ByteSizeLong());
    response.ParseFromArray(shareMemoryBlock.GetDataPoint(), shareMemoryBlock.GetDataSize());
    ASSERT_TRUE(response.status() == 123);

    // ?????????next?????????????????????????????????
    NotifyResultResponse response2;
    response2.set_status(2345);
    ASSERT_TRUE(shareMemoryBlock.PutMessage(response2));
    EXPECT_NE(shareMemoryBlock.GetDataSize(), response2.ByteSizeLong());
    EXPECT_EQ(shareMemoryBlock.GetDataSize(), response.ByteSizeLong());
    response2.ParseFromArray(shareMemoryBlock.GetDataPoint(), shareMemoryBlock.GetDataSize());
    EXPECT_FALSE(response2.status() == 2345);
    EXPECT_TRUE(response2.status() == 123);

    ASSERT_TRUE(shareMemoryBlock.ReleaseBlock());
}

/**
 * @tc.name: share memory
 * @tc.desc: Shared memory PutRaw abnormal test.
 * @tc.type: FUNC
 */
HWTEST_F(SharedMemoryBlockTest, PutRawAbnormal, TestSize.Level1)
{
    ShareMemoryBlock shareMemoryBlock("testname", 4096);
    ASSERT_TRUE(shareMemoryBlock.Valid());

    ASSERT_FALSE(shareMemoryBlock.PutRaw(nullptr, ARRAYSIZE));
    ASSERT_NE(shareMemoryBlock.GetFreeMemory(ARRAYSIZE), nullptr);

    int8_t data[ARRAYSIZE];
    ASSERT_FALSE(shareMemoryBlock.PutRaw(data, 0));
    ASSERT_NE(shareMemoryBlock.GetFreeMemory(0), nullptr);

    ASSERT_FALSE(shareMemoryBlock.PutRaw(data, 4096+1));
    ASSERT_EQ(shareMemoryBlock.GetFreeMemory(4096+1), nullptr);

    ASSERT_TRUE(shareMemoryBlock.ReleaseBlock());
}

bool function(const int8_t data[], uint32_t size)
{
    auto pluginData = std::make_shared<ProfilerPluginData>();
    return pluginData->ParseFromArray(reinterpret_cast<const char*>(data), 6);
}

bool functionErr(const int8_t data[], uint32_t size)
{
    auto pluginData = std::make_shared<ProfilerPluginData>();
    return pluginData->ParseFromArray(reinterpret_cast<const char*>(data), 4096);
}

/**
 * @tc.name: share memory
 * @tc.desc: Shared memory TakeData test.
 * @tc.type: FUNC
 */
HWTEST_F(SharedMemoryBlockTest, TakeData, TestSize.Level1)
{
    ShareMemoryBlock shareMemoryBlock("testname", 4096);
    ASSERT_TRUE(shareMemoryBlock.Valid());

    // ???????????????message
    NotifyResultRequest request;
    ASSERT_TRUE(shareMemoryBlock.PutMessage(request));
    ASSERT_TRUE(shareMemoryBlock.GetDataSize() == 0);
    EXPECT_FALSE(shareMemoryBlock.TakeData(function));

    // ??????????????????message
    shareMemoryBlock.Next();
    NotifyResultResponse response;
    response.set_status(123);
    ASSERT_TRUE(shareMemoryBlock.PutMessage(response));
    EXPECT_FALSE(shareMemoryBlock.GetDataSize() == 0);
    EXPECT_FALSE(shareMemoryBlock.TakeData(function));

    // ????????????message
    shareMemoryBlock.Next();
    ProfilerPluginData data;
    ASSERT_TRUE(shareMemoryBlock.PutMessage(data));
    ASSERT_TRUE(shareMemoryBlock.GetDataSize() == 0);
    EXPECT_FALSE(shareMemoryBlock.TakeData(function));

    // ???????????????message, ???DataSize???????????????
    shareMemoryBlock.Next();
    data.set_name("test");
    ASSERT_TRUE(shareMemoryBlock.PutMessage(data));
    EXPECT_FALSE(shareMemoryBlock.GetDataSize() == 0);
    EXPECT_FALSE(shareMemoryBlock.TakeData(functionErr));

    // ???????????????message,?????????DataSize
    shareMemoryBlock.Next();
    data.set_name("test");
    ASSERT_TRUE(shareMemoryBlock.PutMessage(data));
    EXPECT_FALSE(shareMemoryBlock.GetDataSize() == 0);
    EXPECT_TRUE(shareMemoryBlock.TakeData(function));

    ASSERT_TRUE(shareMemoryBlock.ReleaseBlock());
}
} // namespace

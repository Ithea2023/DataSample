#include <iostream>
#include <atomic>
#include <thread>
#include <vector>
#include "common_code.h"
#include "logging.h"

void holyAllTest()
{
    // in this case, will has 2**7 = 128 case
    const float svc_ratio = 0.7;
    RssnrSubs rssnrSubs;
    Client client(VIDEO_SIMULCAST_INFO);
    client.setSvcRatio(svc_ratio);
    RssnrInfo disable180{180, 180, 0, 0, 0, 0};
    RssnrInfo disable360{360, 360, 0, 0, 0, 0};
    RssnrInfo disable720{720, 720, 0, 0, 0, 0};
    RssnrInfo disable1080{1080, 1080, 0, 0, 0, 0};

    RssnrInfo enable180_1_min{180, 180, 1, 120, 0, true};
    RssnrInfo enable360_1_min{360, 360, 1, 350, 0, true};
    RssnrInfo enable720_1_min{720, 720, 1, 700, 0, true};
    RssnrInfo enable1080_1_min{1080, 1080, 1, 1500, 0, true};

    RssnrInfo enable180_1_max{180, 180, 1, 350, 0, true};
    RssnrInfo enable360_1_max{360, 360, 1, 700, 0, true};
    RssnrInfo enable720_1_max{720, 720, 1, 2500, 0, true};
    RssnrInfo enable1080_1_max{1080, 1080, 1, 4000, 0, true};

    const std::vector<RssnrInfo> disableAllInfo{disable180, disable360, disable720, disable1080};
    const std::vector<RssnrInfo> enableMinInfo{enable180_1_min, enable360_1_min, enable720_1_min, enable1080_1_min};
    const std::vector<RssnrInfo> enableMaxInfo{enable180_1_max, enable360_1_max, enable720_1_max, enable1080_1_max};
    const std::vector<uint32_t> MbpsList{180, 360, 720, 1080};
    RssnrSubs subs;

#define enable_test_1_group
#ifdef enable_test_1_group
    {
        // 180p min test
        subs[180] = std::vector<RssnrInfo>{enable180_1_min};
        client.setRssnrSubs(subs);
        client.setBweBitrate(4000);
        auto it = client.hollyAllIn();
        EQUAL_DE((it[180].allocatedKbps), 120);
        EQUAL_DE(client.reservedBitrateKbps_, 4000 - 120);
        client.setBweBitrate(100);
        it = client.hollyAllIn();
        EQUAL_DE(it[180].allocatedKbps, 100);
        EQUAL_DE(it[360].allocatedKbps, 0);
        EQUAL_DE(it[720].allocatedKbps, 0);
        EQUAL_DE(it[1080].allocatedKbps, 0);

        // 180p max test
        subs[180] = std::vector<RssnrInfo>{enable180_1_max};
        client.setRssnrSubs(subs);
        client.setBweBitrate(4000);
        it = client.hollyAllIn();
        EQUAL_DE(it[180].allocatedKbps, 350);
        client.setBweBitrate(250);
        it = client.hollyAllIn();
        EQUAL_DE(it[180].allocatedKbps, 250);
        client.setBweBitrate(0);
        it = client.hollyAllIn();
        EQUAL_DE(it[180].allocatedKbps, 0);

        // 360p max test
        subs.clear();
        subs[360] = std::vector<RssnrInfo>{enable360_1_max};
        client.setRssnrSubs(subs);
        client.setBweBitrate(4000);
        it = client.hollyAllIn();
        EQUAL_DE((it[360].allocatedKbps), 700);
        EQUAL_DE(client.reservedBitrateKbps_, 4000 - 700);

        {
            // 720 max test
            const uint32_t mbps = 720;
            subs.clear();
            subs[mbps] = std::vector<RssnrInfo>{enable720_1_max};
            client.setRssnrSubs(subs);
            client.setBweBitrate(4000);
            it = client.hollyAllIn();
            EQUAL_DE((it[mbps].allocatedKbps), 2500);
            EQUAL_DE(client.reservedBitrateKbps_, 4000 - 2500);
        }

        {
            // 1080 max test
            const uint32_t mbps = 1080;
            subs.clear();
            subs[mbps] = std::vector<RssnrInfo>{enable1080_1_max};
            client.setRssnrSubs(subs);
            client.setBweBitrate(4000);
            it = client.hollyAllIn();
            EQUAL_DE((it[mbps].allocatedKbps), 4000);
            EQUAL_DE(client.reservedBitrateKbps_, 0);
        }
    }
#endif

#define test_one_layer_with_min_max_bitrate
#ifdef test_one_layer_with_min_max_bitrate
    {
        // subs.clear();
        // subs[1080] = std::vector<RssnrInfo>{ enable1080_1_min, enable1080_1_max};
        // client.setRssnrSubs(subs);
        // client.setBweBitrate(4000);
        // auto it = client.hollyAllIn();
        // EQUAL_DE(it[1080].allocatedKbps, 2500);
        // EQUAL_DE(it[720].allocatedKbps, 1500);
        // EQUAL_DE(client.reservedBitrateKbps_, 0);
        // return;

        auto bweChanged = [&](uint32_t bweKbps)
        {
            for (int i = 0; i < 4; ++i)
            {
                subs.clear();
                subs[MbpsList[i]] = std::vector<RssnrInfo>{enableMaxInfo[i], enableMinInfo[i]};
                client.setRssnrSubs(subs);
                client.setBweBitrate(bweKbps);
                auto it = client.hollyAllIn();

                if (i > 0 && bweKbps >= (enableMinInfo[i].targetKbps + enableMaxInfo[i].targetKbps))
                {
                    EQUAL_DE_INFO(it[MbpsList[i]].allocatedKbps, enableMaxInfo[i].targetKbps, i);
                    EQUAL_DE_INFO(it[MbpsList[i-1]].allocatedKbps, enableMinInfo[i].targetKbps, i);
                    EQUAL_DE_INFO(client.reservedBitrateKbps_, bweKbps -
                                  (enableMinInfo[i].targetKbps + enableMaxInfo[i].targetKbps), i);
                }
                else if (i > 0 && bweKbps >= enableMinInfo[i].targetKbps * 2)
                {
                    EQUAL_DE_INFO(it[MbpsList[i]].allocatedKbps, bweKbps - enableMinInfo[i].targetKbps, i);
                    EQUAL_DE_INFO(it[MbpsList[i - 1]].allocatedKbps, enableMinInfo[i].targetKbps, i);
                    EQUAL_DE_INFO(client.reservedBitrateKbps_, 0, i);
                }
                else
                {
                    const uint32_t allocated = std::min<uint32_t>(bweKbps,
                                                                  enableMinInfo[i].targetKbps / svc_ratio);

                    EQUAL_DE_INFO((it[MbpsList[i]].allocatedKbps), allocated, i);
                    EQUAL_DE_INFO(client.reservedBitrateKbps_, bweKbps - allocated, i);
                }
            }
        };
        // bweChanged(4000);
        // bweChanged(2500);
        // bweChanged(1800);
        // bweChanged(1000);
        // bweChanged(500);
        // bweChanged(300);
        // bweChanged(0);
    }

#endif

#define test_two_near_layer_with_min_or_max_bitrate
#ifdef test_two_near_layer_with_min_or_max_bitrate
    {
        auto bweAndFourSubs = [&](uint32_t bweKbps, uint32_t index)
        {
            subs.clear();
            subs[MbpsList[index]] = std::vector<RssnrInfo>{enableMaxInfo[index], enableMinInfo[index]};
            subs[MbpsList[index + 1]] = std::vector<RssnrInfo>{enableMinInfo[index + 1], enableMaxInfo[index + 1]};
            client.setRssnrSubs(subs);
            client.setBweBitrate(bweKbps);
            return client.hollyAllIn();
        };
        SubscriptionList it;

        it = bweAndFourSubs(4000, 0);
        EQUAL_DE(client.reservedBitrateKbps_, 3129);
        EQUAL_DE(it[MbpsList[0]].allocatedKbps, static_cast<uint32_t>(enableMinInfo[0].targetKbps / svc_ratio));
        EQUAL_DE(it[MbpsList[1]].allocatedKbps, static_cast<uint32_t>(enableMaxInfo[1].targetKbps));

        it = bweAndFourSubs(1000, 0);
        EQUAL_DE(client.reservedBitrateKbps_, 329);
        EQUAL_DE(it[MbpsList[0]].allocatedKbps, static_cast<uint32_t>(enableMinInfo[0].targetKbps / svc_ratio));
        EQUAL_DE(it[MbpsList[1]].allocatedKbps, static_cast<uint32_t>(enableMaxInfo[1].targetKbps));

        it = bweAndFourSubs(500, 0);
        EQUAL_DE(client.reservedBitrateKbps_, 0);
        EQUAL_DE(it[MbpsList[0]].allocatedKbps, 150);
        EQUAL_DE(it[MbpsList[1]].allocatedKbps, 500 - 150); //TODO: need check

        it = bweAndFourSubs(300, 0);
        EQUAL_DE(client.reservedBitrateKbps_, 0);
        EQUAL_DE(it[MbpsList[0]].allocatedKbps, static_cast<uint32_t>(enableMinInfo[0].targetKbps / svc_ratio));
        EQUAL_DE(it[MbpsList[1]].allocatedKbps, 129);

        it = bweAndFourSubs(4000, 1);
        EQUAL_DE(client.reservedBitrateKbps_, 450);
        EQUAL_DE(it[MbpsList[0]].allocatedKbps, static_cast<uint32_t>(enableMaxInfo[0].targetKbps));
        EQUAL_DE(it[MbpsList[1]].allocatedKbps, static_cast<uint32_t>(enableMaxInfo[1].targetKbps));
        EQUAL_DE(it[MbpsList[2]].allocatedKbps, static_cast<uint32_t>(enableMaxInfo[2].targetKbps));
    }
#endif
}

void tryAllocationTest()
{
    Client client(VIDEO_SIMULCAST_INFO);
    client.setSvcRatio(0.7);
    SubscriptionList subsInput;
    subsInput[180] = subSimulcastConfig{
        180, 0, BitrateSuber(),
        BitrateSuber(), 0, 0
    };
    subsInput[360] = subSimulcastConfig{
        360, 0, BitrateSuber(),
        BitrateSuber(), 0, 0
    };
    subsInput[720] = subSimulcastConfig{
        720, 0, BitrateSuber(),
        BitrateSuber(), 0, 0
    };
    subsInput[1080] = subSimulcastConfig{
        1080, 0, BitrateSuber(),
        BitrateSuber(), 0, 0
    };

    bool res = false;
    auto copySubsInput = subsInput;

    // empty test
    copySubsInput = subsInput;
    res = client.tryAllocation(copySubsInput, 4000);
    EQUAL_DE(res, false);
    EQUAL_DE(copySubsInput[180].allocatedKbps, 0);

    // sub 180p
    copySubsInput = subsInput;
    copySubsInput[180] = subSimulcastConfig{
        180, 1, BitrateSuber{350, 1},
        BitrateSuber{350, 0}, 0, 0
    };
    res = client.tryAllocation(copySubsInput, 4000);
    EQUAL_DE(res, true);
    EQUAL_DE(copySubsInput[180].allocatedKbps, 350);

    // sub 180p one layer
    copySubsInput = subsInput;
    copySubsInput[180] = subSimulcastConfig{
        180, 1, BitrateSuber{350, 1},
        BitrateSuber{350, 0}, 0, 0
    };
    res = client.tryAllocation(copySubsInput, 250);
    EQUAL_DE(res, false);
    EQUAL_DE(copySubsInput[180].allocatedKbps, 200);

    // sub 180p tow layer
    copySubsInput = subsInput;
    copySubsInput[180] = subSimulcastConfig{
        180, 1, BitrateSuber{350, 1},
        BitrateSuber{120, 1}, 0, 0
    };
    res = client.tryAllocation(copySubsInput, 250);
    EQUAL_DE(res, true);
    EQUAL_DE(copySubsInput[180].allocatedKbps, 171);

    // sub 180p tow layer
    copySubsInput = subsInput;
    copySubsInput[180] = subSimulcastConfig{
        180, 1, BitrateSuber{350, 1},
        BitrateSuber{120, 1}, 0, 0
    };
    res = client.tryAllocation(copySubsInput, 130);
    EQUAL_DE(res, false);
    EQUAL_DE(copySubsInput[180].allocatedKbps, 120);

    // sub 180p tow layer
    copySubsInput = subsInput;
    copySubsInput[180] = subSimulcastConfig{
        180, 1, BitrateSuber{350, 1},
        BitrateSuber{120, 1}, 0, 0
    };
    res = client.tryAllocation(copySubsInput, 600);
    EQUAL_DE(res, true);
    EQUAL_DE(copySubsInput[180].allocatedKbps, 171);

    // sub 180p tow layer
    copySubsInput = subsInput;
    copySubsInput[180] = subSimulcastConfig{
        180, 1, BitrateSuber{350, 1},
        BitrateSuber{200, 1}, 0, 0
    };
    res = client.tryAllocation(copySubsInput, 600);
    EQUAL_DE(res, true);
    EQUAL_DE(copySubsInput[180].allocatedKbps, 285);

    // sub 180p, 360p one layer
    copySubsInput = subsInput;
    copySubsInput[180] = subSimulcastConfig{
        180, 1, BitrateSuber{200, 1},
        BitrateSuber{200, 0}, 0, 0
    };
    copySubsInput[360] = subSimulcastConfig{
        180, 1, BitrateSuber{450, 1},
        BitrateSuber{450, 0}, 0, 0
    };
    res = client.tryAllocation(copySubsInput, 600);
    EQUAL_DE(res, false);
    EQUAL_DE(copySubsInput[180].allocatedKbps, 200);
    EQUAL_DE(copySubsInput[360].allocatedKbps, 350);

    // sub 180p, 360p one layer
    copySubsInput = subsInput;
    copySubsInput[180] = subSimulcastConfig{
        180, 1, BitrateSuber{200, 1},
        BitrateSuber{200, 0}, 0, 0
    };
    copySubsInput[360] = subSimulcastConfig{
        180, 1, BitrateSuber{450, 1},
        BitrateSuber{450, 0}, 0, 0
    };
    res = client.tryAllocation(copySubsInput, 700);
    EQUAL_DE(res, true);
    EQUAL_DE(copySubsInput[180].allocatedKbps, 200);
    EQUAL_DE(copySubsInput[360].allocatedKbps, 450);

    // sub 180p, 360p two layer
    copySubsInput = subsInput;
    copySubsInput[180] = subSimulcastConfig{
        180, 1, BitrateSuber{200, 1},
        BitrateSuber{120, 1}, 0, 0
    };
    copySubsInput[360] = subSimulcastConfig{
        180, 1, BitrateSuber{450, 1},
        BitrateSuber{350, 0}, 0, 0
    };
    res = client.tryAllocation(copySubsInput, 700);
    EQUAL_DE(res, true);
    EQUAL_DE(copySubsInput[180].allocatedKbps, 171);
    EQUAL_DE(copySubsInput[360].allocatedKbps, 450);
}

void tryAdjustSubscriptionTest()
{
    Client client(VIDEO_SIMULCAST_INFO);
    client.setSvcRatio(0.7);
    SubscriptionList subsInput;
    subsInput[180] = subSimulcastConfig{
        180, 0, BitrateSuber(),
        BitrateSuber(), 0, 0
    };
    subsInput[360] = subSimulcastConfig{
        360, 0, BitrateSuber(),
        BitrateSuber(), 0, 0
    };
    subsInput[720] = subSimulcastConfig{
        720, 0, BitrateSuber(),
        BitrateSuber(), 0, 0
    };
    subsInput[1080] = subSimulcastConfig{
        1080, 0, BitrateSuber(),
        BitrateSuber(), 0, 0
    };

    bool res = false;
    auto copySubsInput = subsInput;

    // empty test
    copySubsInput = subsInput;
    res = client.tryAdjustSubscription(copySubsInput);
    EQUAL_DE(res, false);
    EQUAL_DE(copySubsInput[180].maxBitrateSuber.bitrateKbps, 0);
    EQUAL_DE(copySubsInput[180].minBitrateSuer.bitrateKbps, 0);
    EQUAL_DE(copySubsInput[360].maxBitrateSuber.bitrateKbps, 0);
    EQUAL_DE(copySubsInput[360].minBitrateSuer.bitrateKbps, 0);
    EQUAL_DE(copySubsInput[720].maxBitrateSuber.bitrateKbps, 0);
    EQUAL_DE(copySubsInput[720].minBitrateSuer.bitrateKbps, 0);
    EQUAL_DE(copySubsInput[1080].maxBitrateSuber.bitrateKbps, 0);
    EQUAL_DE(copySubsInput[1080].minBitrateSuer.bitrateKbps, 0);

    // sub 180p one bitrate
    copySubsInput = subsInput;
    copySubsInput[180] = subSimulcastConfig{
        180, 1, BitrateSuber{350, 1},
        BitrateSuber{350, 0}, 0, 0
    };
    res = client.tryAdjustSubscription(copySubsInput);
    EQUAL_DE(res, false);
    EQUAL_DE(copySubsInput[180].maxBitrateSuber.bitrateKbps, 350);
    EQUAL_DE(copySubsInput[180].minBitrateSuer.bitrateKbps, 350);

    // sub 180p two bitrate
    copySubsInput = subsInput;
    copySubsInput[180] = subSimulcastConfig{
        180, 1, BitrateSuber{350, 1},
        BitrateSuber{200, 1}, 0, 0
    };
    res = client.tryAdjustSubscription(copySubsInput);
    EQUAL_DE(res, false);
    EQUAL_DE(copySubsInput[180].maxBitrateSuber.bitrateKbps, 350);
    EQUAL_DE(copySubsInput[180].minBitrateSuer.bitrateKbps, 200);

    // sub 180p two bitrate
    copySubsInput = subsInput;
    copySubsInput[180] = subSimulcastConfig{
        180, 1, BitrateSuber{350, 1},
        BitrateSuber{120, 1}, 0, 0
    };
    res = client.tryAdjustSubscription(copySubsInput);
    EQUAL_DE(res, false);
    EQUAL_DE(copySubsInput[180].maxBitrateSuber.bitrateKbps, 350);
    EQUAL_DE(copySubsInput[180].maxBitrateSuber.count, 1);
    EQUAL_DE(copySubsInput[180].minBitrateSuer.bitrateKbps, 120);

    // sub 360p two bitrate
    copySubsInput = subsInput;
    copySubsInput[360] = subSimulcastConfig{
        360, 1, BitrateSuber{700, 1},
        BitrateSuber{350, 1}, 0, 0
    };
    res = client.tryAdjustSubscription(copySubsInput);
    EQUAL_DE(res, true);
    EQUAL_DE(copySubsInput[180].maxBitrateSuber.bitrateKbps, 350);
    EQUAL_DE(copySubsInput[180].maxBitrateSuber.count, 1);
    EQUAL_DE(copySubsInput[180].minBitrateSuer.bitrateKbps, 350);
    EQUAL_DE(copySubsInput[180].minBitrateSuer.count, 0);

    EQUAL_DE(copySubsInput[360].maxBitrateSuber.bitrateKbps, 700);
    EQUAL_DE(copySubsInput[360].maxBitrateSuber.count, 1);
    EQUAL_DE(copySubsInput[360].minBitrateSuer.bitrateKbps, 700);
    EQUAL_DE(copySubsInput[360].minBitrateSuer.count, 0);

    // sub 720 two bitrate, 360p one bitrate
    copySubsInput = subsInput;
    copySubsInput[720] = subSimulcastConfig{
        720, 1, BitrateSuber{2000, 1},
        BitrateSuber{800, 1}, 0, 0
    };
    copySubsInput[360] = subSimulcastConfig{
        360, 1, BitrateSuber{700, 1},
        BitrateSuber{700, 0}, 0, 0
    };
    res = client.tryAdjustSubscription(copySubsInput);
    EQUAL_DE(res, true);
    EQUAL_DE(copySubsInput[720].maxBitrateSuber.bitrateKbps, 2000);
    EQUAL_DE(copySubsInput[720].maxBitrateSuber.count, 1);
    EQUAL_DE(copySubsInput[720].minBitrateSuer.bitrateKbps, 2000);
    EQUAL_DE(copySubsInput[720].minBitrateSuer.count, 0);

    EQUAL_DE(copySubsInput[360].maxBitrateSuber.bitrateKbps, 700);
    EQUAL_DE(copySubsInput[360].maxBitrateSuber.count, 2);
    EQUAL_DE(copySubsInput[360].minBitrateSuer.bitrateKbps, 700);
    EQUAL_DE(copySubsInput[360].minBitrateSuer.count, 0);

    // sub 720 two bitrate, 360p one bitrate, more people
    copySubsInput = subsInput;
    copySubsInput[720] = subSimulcastConfig{
        720, 1, BitrateSuber{2000, 1},
        BitrateSuber{800, 10}, 0, 0
    };
    copySubsInput[360] = subSimulcastConfig{
        360, 1, BitrateSuber{700, 1},
        BitrateSuber{700, 0}, 0, 0
    };
    res = client.tryAdjustSubscription(copySubsInput);
    EQUAL_DE(res, false);
    EQUAL_DE(copySubsInput[720].maxBitrateSuber.bitrateKbps, 2000);
    EQUAL_DE(copySubsInput[720].maxBitrateSuber.count, 1);
    EQUAL_DE(copySubsInput[720].minBitrateSuer.bitrateKbps, 800);
    EQUAL_DE(copySubsInput[720].minBitrateSuer.count, 10);

    EQUAL_DE(copySubsInput[360].maxBitrateSuber.bitrateKbps, 700);
    EQUAL_DE(copySubsInput[360].maxBitrateSuber.count, 1);
    EQUAL_DE(copySubsInput[360].minBitrateSuer.bitrateKbps, 700);
    EQUAL_DE(copySubsInput[360].minBitrateSuer.count, 0);

    // sub 720 two bitrate, 360p one bitrate
    copySubsInput = subsInput;
    copySubsInput[720] = subSimulcastConfig{
        720, 1, BitrateSuber{2000, 1},
        BitrateSuber{800, 1}, 0, 0
    };
    copySubsInput[360] = subSimulcastConfig{
        360, 1, BitrateSuber{700, 1},
        BitrateSuber{400, 1}, 0, 0
    };
    res = client.tryAdjustSubscription(copySubsInput);
    EQUAL_DE(res, true);
    EQUAL_DE(copySubsInput[720].maxBitrateSuber.bitrateKbps, 2000);
    EQUAL_DE(copySubsInput[720].maxBitrateSuber.count, 1);
    EQUAL_DE(copySubsInput[720].minBitrateSuer.bitrateKbps, 2000);
    EQUAL_DE(copySubsInput[720].minBitrateSuer.count, 0);

    EQUAL_DE(copySubsInput[360].maxBitrateSuber.bitrateKbps, 700);
    EQUAL_DE(copySubsInput[360].maxBitrateSuber.count, 2);
    EQUAL_DE(copySubsInput[360].minBitrateSuer.bitrateKbps, 700);
    EQUAL_DE(copySubsInput[360].minBitrateSuer.count, 0);

    EQUAL_DE(copySubsInput[180].maxBitrateSuber.bitrateKbps, 350);
    EQUAL_DE(copySubsInput[180].maxBitrateSuber.count, 1);
    EQUAL_DE(copySubsInput[180].minBitrateSuer.bitrateKbps, 350);
    EQUAL_DE(copySubsInput[180].minBitrateSuer.count, 0);

    // sub 1080p two bitrate, sub 720 one bitrate, 360p two bitrate, sub 180p min bitrate
    copySubsInput = subsInput;
    copySubsInput[1080] = subSimulcastConfig{
        1080, 1, BitrateSuber{4000, 1},
        BitrateSuber{2000, 1}, 0, 0
    };
    copySubsInput[720] = subSimulcastConfig{
        720, 1, BitrateSuber{1800, 1},
        BitrateSuber{1800, 0}, 0, 0
    };
    copySubsInput[360] = subSimulcastConfig{
        360, 1, BitrateSuber{700, 1},
        BitrateSuber{400, 1}, 0, 0
    };
    copySubsInput[180] = subSimulcastConfig{
        180, 1, BitrateSuber{180, 1},
        BitrateSuber{180, 0}, 0, 0
    };
    res = client.tryAdjustSubscription(copySubsInput);
    EQUAL_DE(res, true);
    EQUAL_DE(copySubsInput[1080].maxBitrateSuber.bitrateKbps, 4000);
    EQUAL_DE(copySubsInput[1080].maxBitrateSuber.count, 1);
    EQUAL_DE(copySubsInput[1080].minBitrateSuer.bitrateKbps, 4000);
    EQUAL_DE(copySubsInput[1080].minBitrateSuer.count, 0);

    EQUAL_DE(copySubsInput[720].maxBitrateSuber.bitrateKbps, 2000);
    EQUAL_DE(copySubsInput[720].maxBitrateSuber.count, 2);
    EQUAL_DE(copySubsInput[720].minBitrateSuer.bitrateKbps, 1800);
    EQUAL_DE(copySubsInput[720].minBitrateSuer.count, 0);

    EQUAL_DE(copySubsInput[360].maxBitrateSuber.bitrateKbps, 700);
    EQUAL_DE(copySubsInput[360].maxBitrateSuber.count, 1);
    EQUAL_DE(copySubsInput[360].minBitrateSuer.bitrateKbps, 700);
    EQUAL_DE(copySubsInput[360].minBitrateSuer.count, 0);

    EQUAL_DE(copySubsInput[180].maxBitrateSuber.bitrateKbps, 350);
    EQUAL_DE(copySubsInput[180].maxBitrateSuber.count, 1);
    EQUAL_DE(copySubsInput[180].minBitrateSuer.bitrateKbps, 180);
    EQUAL_DE(copySubsInput[180].minBitrateSuer.count, 1);

    // sub 1080p two bitrate, sub 720 two bitrate, 360p one bitrate, sub 180p null
    copySubsInput = subsInput;
    copySubsInput[1080] = subSimulcastConfig{
        1080, 1, BitrateSuber{4000, 1},
        BitrateSuber{2000, 1}, 0, 0
    };
    copySubsInput[720] = subSimulcastConfig{
        720, 1, BitrateSuber{1800, 1},
        BitrateSuber{800, 5}, 0, 0
    };
    copySubsInput[360] = subSimulcastConfig{
        360, 1, BitrateSuber{700, 1},
        BitrateSuber{700, 0}, 0, 0
    };
    copySubsInput[180] = subSimulcastConfig{
        180, 1, BitrateSuber{0, 0},
        BitrateSuber{0, 0}, 0, 0
    };
    res = client.tryAdjustSubscription(copySubsInput);
    EQUAL_DE(res, true);
    EQUAL_DE(copySubsInput[1080].maxBitrateSuber.bitrateKbps, 4000);
    EQUAL_DE(copySubsInput[1080].maxBitrateSuber.count, 1);
    EQUAL_DE(copySubsInput[1080].minBitrateSuer.bitrateKbps, 4000);
    EQUAL_DE(copySubsInput[1080].minBitrateSuer.count, 0);

    EQUAL_DE(copySubsInput[720].maxBitrateSuber.bitrateKbps, 2000);
    EQUAL_DE(copySubsInput[720].maxBitrateSuber.count, 2);
    EQUAL_DE(copySubsInput[720].minBitrateSuer.bitrateKbps, 800);
    EQUAL_DE(copySubsInput[720].minBitrateSuer.count, 5);

    EQUAL_DE(copySubsInput[360].maxBitrateSuber.bitrateKbps, 700);
    EQUAL_DE(copySubsInput[360].maxBitrateSuber.count, 1);
    EQUAL_DE(copySubsInput[360].minBitrateSuer.bitrateKbps, 700);
    EQUAL_DE(copySubsInput[360].minBitrateSuer.count, 0);

    EQUAL_DE(copySubsInput[180].maxBitrateSuber.bitrateKbps, 0);
    EQUAL_DE(copySubsInput[180].maxBitrateSuber.count, 0);
    EQUAL_DE(copySubsInput[180].minBitrateSuer.bitrateKbps, 0);
    EQUAL_DE(copySubsInput[180].minBitrateSuer.count, 0);
}

void allocateBitrateTest() { }

void AllTest()
{
    Client client(VIDEO_SIMULCAST_INFO);
    client.setSvcRatio(0.7);
    SubscriptionList subsInput;
    subsInput[180] = subSimulcastConfig{
        180, 0, BitrateSuber(),
        BitrateSuber(), 0, 0
    };
    subsInput[360] = subSimulcastConfig{
        360, 0, BitrateSuber(),
        BitrateSuber(), 0, 0
    };
    subsInput[720] = subSimulcastConfig{
        720, 0, BitrateSuber(),
        BitrateSuber(), 0, 0
    };
    subsInput[1080] = subSimulcastConfig{
        1080, 0, BitrateSuber(),
        BitrateSuber(), 0, 0
    };

    bool res = false;
    auto copySubsInput = subsInput;
    SubscriptionList resAllocationOut;

    // empty test
    copySubsInput = subsInput;
    resAllocationOut = client.calculateAll(copySubsInput, 4000);
    EQUAL_DE(resAllocationOut[180].allocatedKbps, 0);
    EQUAL_DE(client.reservedBitrateKbps_, 4000);

    // sub 180p
    copySubsInput = subsInput;
    copySubsInput[180] = subSimulcastConfig{
        180, 1, BitrateSuber{350, 1},
        BitrateSuber{350, 0}, 0, 0
    };
    resAllocationOut = client.calculateAll(copySubsInput, 4000);
    EQUAL_DE(resAllocationOut[180].allocatedKbps, 350);
    EQUAL_DE(client.reservedBitrateKbps_, 4000-350);

    // sub 180p 0k
    copySubsInput = subsInput;
    copySubsInput[180] = subSimulcastConfig{
        180, 1, BitrateSuber{350, 1},
        BitrateSuber{350, 0}, 0, 0
    };
    resAllocationOut = client.calculateAll(copySubsInput, 0);
    EQUAL_DE(resAllocationOut[180].allocatedKbps, 0);
    EQUAL_DE(client.reservedBitrateKbps_, 0);

    // sub 180p two bitrate
    copySubsInput = subsInput;
    copySubsInput[180] = subSimulcastConfig{
        180, 1, BitrateSuber{350, 1},
        BitrateSuber{130, 1}, 0, 0
    };
    resAllocationOut = client.calculateAll(copySubsInput, 4000);
    EQUAL_DE(resAllocationOut[180].allocatedKbps, 185);
    EQUAL_DE(client.reservedBitrateKbps_, 4000-185);

    // sub 180p two bitrate low bitrate
    copySubsInput = subsInput;
    copySubsInput[180] = subSimulcastConfig{
        180, 1, BitrateSuber{350, 1},
        BitrateSuber{130, 1}, 0, 0
    };
    resAllocationOut = client.calculateAll(copySubsInput, 120);
    EQUAL_DE(resAllocationOut[180].allocatedKbps, 120);
    EQUAL_DE(client.reservedBitrateKbps_, 0);

    // sub 180, 360
    copySubsInput = subsInput;
    copySubsInput[180] = subSimulcastConfig{
        180, 1, BitrateSuber{350, 1},
        BitrateSuber{130, 1}, 0, 0
    };
    copySubsInput[360] = subSimulcastConfig{
        360, 1, BitrateSuber{700, 1},
        BitrateSuber{400, 1}, 0, 0
    };
    resAllocationOut = client.calculateAll(copySubsInput, 4000);
    EQUAL_DE(resAllocationOut[360].allocatedKbps, 700);
    EQUAL_DE(resAllocationOut[180].allocatedKbps, 185);
    EQUAL_DE(client.reservedBitrateKbps_, 4000 - 700 - 185);

    // sub 180, 360 min bitrate
    copySubsInput = subsInput;
    copySubsInput[180] = subSimulcastConfig{
        180, 1, BitrateSuber{350, 1},
        BitrateSuber{130, 1}, 0, 0
    };
    copySubsInput[360] = subSimulcastConfig{
        360, 1, BitrateSuber{700, 1},
        BitrateSuber{400, 1}, 0, 0
    };
    resAllocationOut = client.calculateAll(copySubsInput, 850);
    EQUAL_DE(resAllocationOut[360].allocatedKbps, 665);
    EQUAL_DE(resAllocationOut[180].allocatedKbps, 185);
    EQUAL_DE(client.reservedBitrateKbps_, 850 - 665 - 185);

    // sub 360 bitrate
    copySubsInput = subsInput;
    copySubsInput[360] = subSimulcastConfig{
        360, 1, BitrateSuber{700, 1},
        BitrateSuber{400, 1}, 0, 0
    };
    resAllocationOut = client.calculateAll(copySubsInput, 850);
    EQUAL_DE(resAllocationOut[360].allocatedKbps, 500);
    EQUAL_DE(resAllocationOut[180].allocatedKbps, 350);
    EQUAL_DE(client.reservedBitrateKbps_, 850 - 500 - 350);

    // sub 360 bitrate one layer
    copySubsInput = subsInput;
    copySubsInput[360] = subSimulcastConfig{
        360, 1, BitrateSuber{700, 1},
        BitrateSuber{700, 0}, 0, 0
    };
    resAllocationOut = client.calculateAll(copySubsInput, 850);
    EQUAL_DE(resAllocationOut[360].allocatedKbps, 700);
    EQUAL_DE(resAllocationOut[180].allocatedKbps, 0);
    EQUAL_DE(client.reservedBitrateKbps_, 850 - 700);

    // sub 360, 720 min bitrate 4000k
    copySubsInput = subsInput;
    copySubsInput[360] = subSimulcastConfig{
        360, 1, BitrateSuber{500, 1},
        BitrateSuber{500, 0}, 0, 0
    };
    copySubsInput[720] = subSimulcastConfig{
        720, 1, BitrateSuber{2500, 1},
        BitrateSuber{800, 1}, 0, 0
    };
    resAllocationOut = client.calculateAll(copySubsInput, 4000);
    EQUAL_DE(resAllocationOut[360].allocatedKbps, 700);
    EQUAL_DE(resAllocationOut[720].allocatedKbps, 2500);
    EQUAL_DE(client.reservedBitrateKbps_, 4000 - 2500 -700);

    // sub 360, 720 min bitrate 2500k
    // TODO: wanderful
    copySubsInput = subsInput;
    copySubsInput[360] = subSimulcastConfig{
        360, 1, BitrateSuber{400, 1},
        BitrateSuber{400, 0}, 0, 0
    };
    copySubsInput[720] = subSimulcastConfig{
        720, 1, BitrateSuber{2500, 1},
        BitrateSuber{800, 1}, 0, 0
    };
    resAllocationOut = client.calculateAll(copySubsInput, 4000);
    EQUAL_DE(resAllocationOut[720].allocatedKbps, 2500);
    EQUAL_DE(resAllocationOut[360].allocatedKbps, 700);
    EQUAL_DE(resAllocationOut[180].allocatedKbps, 350);
    EQUAL_DE(client.reservedBitrateKbps_, 4000 - 2500 - 700 - 350);

    // sub 360, 720 min bitrate 1700k
    // TODO: wanderful
    copySubsInput = subsInput;
    copySubsInput[360] = subSimulcastConfig{
        360, 1, BitrateSuber{400, 1},
        BitrateSuber{400, 0}, 0, 0
    };
    copySubsInput[720] = subSimulcastConfig{
        720, 1, BitrateSuber{2500, 1},
        BitrateSuber{800, 1}, 0, 0
    };
    resAllocationOut = client.calculateAll(copySubsInput, 1700);
    EQUAL_DE(resAllocationOut[720].allocatedKbps, 1142);
    EQUAL_DE(resAllocationOut[360].allocatedKbps, 400);
    EQUAL_DE(resAllocationOut[180].allocatedKbps, 0);
    EQUAL_DE(client.reservedBitrateKbps_, 1700 - 1142 - 400);

    // sub 360, 720 min bitrate 1000k
    // TODO: wanderful awesome
    copySubsInput = subsInput;
    copySubsInput[360] = subSimulcastConfig{
        360, 1, BitrateSuber{400, 1},
        BitrateSuber{400, 0}, 0, 0
    };
    copySubsInput[720] = subSimulcastConfig{
        720, 1, BitrateSuber{2500, 1},
        BitrateSuber{800, 1}, 0, 0
    };
    resAllocationOut = client.calculateAll(copySubsInput, 1000);
    EQUAL_DE(resAllocationOut[720].allocatedKbps, 0);
    EQUAL_DE(resAllocationOut[360].allocatedKbps, 650);
    EQUAL_DE(resAllocationOut[180].allocatedKbps, 350);
    EQUAL_DE(client.reservedBitrateKbps_, 0);
}
int main()
{
    // tryAllocationTest();
    // tryAdjustSubscriptionTest();
    AllTest();
}

#ifndef COMMON_CODE_H_
#define COMMON_CODE_H_

#include <chrono>
#include <deque>
#include <functional>
#include <memory>
#include <queue>
#include <unordered_map>
#include <map>

class Timer
{
public:
    Timer()
        : beg_(clock_::now()) { }

    void
    reset()
    {
        beg_ = clock_::now();
    }

    double
    elapsed_ms() const
    {
        return std::chrono::duration_cast<second_>(clock_::now() - beg_).count() * 1000;
    }

    double
    elapsed_ns() const
    {
        return std::chrono::duration_cast<second_>(clock_::now() - beg_).count() * 10000000;
    }

private:
    typedef std::chrono::high_resolution_clock clock_;
    typedef std::chrono::duration<double, std::ratio<1>> second_;
    std::chrono::time_point<clock_> beg_;
};

namespace base
{
typedef uint32_t Mbps;
typedef uint32_t Ssrc;
typedef uint32_t bitrateKbps;
typedef uint32_t count;
}

struct RssnrInfo
{
    base::Ssrc ssrc = 0;
    base::Mbps mbps = 0;
    uint32_t count = 0;
    uint32_t targetKbps = 0;
    uint32_t fps = 0;
    bool enable = false;
};

struct SubscribeInfo
{
    // base::Mbps mbps = 0;
    bool enable = false;
    uint32_t neededBitrateKbps = 0;
    uint32_t allocatedBitrateKbps = 0;
};

struct Resolution
{
    uint32_t width;
    uint32_t height;
};

enum SIMULCAST_LAYER
{
    LAYER_90P = 90,
    LAYER_180P = 180,
    LAYER_270P = 270,
    LAYER_360P = 360,
    LAYER_540P = 540,
    LAYER_720P = 720,
    LAYER_900P = 900,
    LAYER_1080P = 1080,
    LAYER_1440P = 1440,
};

struct SimulcastBitrateInfo
{
    Resolution resolution;
    uint32_t expectRateBps;
    uint32_t minBitrateBps; //lower switch rate bps
    uint32_t upperSwitchRateBps;
    uint32_t maxBitrateBps;
    SIMULCAST_LAYER layer;
    uint32_t frameRate;
};

struct BitrateSuber
{
    uint32_t bitrateKbps = 0;
    uint32_t count = 0;
};

struct subSimulcastConfig
{
    base::Mbps mbps = 0;
    bool enable = false;
    BitrateSuber maxBitrateSuber{0, 0};
    BitrateSuber minBitrateSuer{0, 0};
    base::bitrateKbps expectBitrateKbps = 0;
    base::bitrateKbps allocatedKbps = 0;
};

typedef std::map<base::Mbps, SubscribeInfo> SubscribeSubs;
typedef std::map<base::Mbps, std::vector<RssnrInfo>> RssnrSubs;

typedef std::map<base::Mbps, subSimulcastConfig> SubscriptionList;
typedef std::map<uint32_t, SimulcastBitrateInfo> SimulcastAbility;

static SimulcastAbility VIDEO_SIMULCAST_INFO({
    {180, {{320, 180}, 200 * 1000, 120 * 1000, 120 * 1000, 350 * 1000, LAYER_180P, 15}},
    {360, {{640, 360}, 500 * 1000, 350 * 1000, 420 * 1000, 700 * 1000, LAYER_360P, 24}},
    {720, {{1280, 720}, 1500 * 1000, 700 * 1000, 850 * 1000, 2500 * 1000, LAYER_720P, 30}},
    {1080, {{1920, 1080}, 2500 * 1000, 1500 * 1000, 2500 * 1000, 4000 * 1000, LAYER_1080P, 30}},

});

class ServerSubs
{
public:
    ServerSubs(std::list<base::Mbps> mbpsList);

    ServerSubs() = delete;

    ~ServerSubs() = default;

    bool AddSubs(base::Mbps mbps, uint32_t bitrate);

    bool removeSubs(base::Mbps mbps, uint32_t bitrate);

    std::list<base::Mbps> getMbpsList() const;

    bool setMbpsState(base::Mbps mbps, bool state);

    std::list<base::Mbps> mbpsList_;
    RssnrSubs rssnrSubs_;
};

class Client
{
public:
    Client(SimulcastAbility ability);

    ~Client() = default;

    SubscriptionList setRssnrSubs(RssnrSubs subs);

    SubscriptionList hollyAllIn();

    void setSvcRatio(float ratio);

    SubscribeSubs calculateSubscribeSubs();

    uint32_t calculateUprightBitrateDiff(SubscribeSubs subs);

    void setBweBitrate(uint32_t bitrateKbps);

    SubscribeSubs TryAllocateBweBitrate(SubscribeSubs subscribeSubs);

    RssnrSubs AdjustSubscription();

    uint32_t getAllocateBitrateDiff() const;

    void PrintSubscribeSubs(SubscribeSubs subs, uint32_t bweKbps, uint32_t reservedKbps) const;

    SubscriptionList calculateAll(SubscriptionList subList, uint32_t bweKbps);

    // in: allocated subs, sum of all bitrate
    // try allocation reserved bitrate = 0, return subs
    uint32_t allocateBitrate(SubscriptionList& subs, uint32_t sumOfBitrateKbps);

    // in:   subs, allocation is 0, sum of all bitrate
    // out:  false: need adjust subs, true: not need adjust subs
    // out&: allocated subs, reserved bitrate
    // allocte three times
    bool tryAllocation(SubscriptionList& subs, uint32_t bitrateKbps);

    // in:   subs, allocation is 0, sum of all bitrate, not use
    // out:  false: not adjust subs, true: adjust subs
    // out&: adjusted subs
    // do not care allocated bitrate
    bool tryAdjustSubscription(SubscriptionList& subs);


    uint32_t lastAllocateDiffKbps_;
    uint32_t bweBitrateKbps_;

    SimulcastAbility simulcastAbility_;
    //subscribe list for server rssnr module
    RssnrSubs rssnrSubs_;
    //local subs info
    SubscribeSubs subscribeSubs_;

    SubscriptionList subscriptionList_;

    float svcRatio_ = 0.7;

    //TODO: just for test, can remove
    uint32_t reservedBitrateKbps_ = 0;
};

#endif

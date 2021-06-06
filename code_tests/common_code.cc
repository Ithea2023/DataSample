#include "common_code.h"
#include <sstream>
#include <algorithm>
#include <bitset>
#include <functional>
#include <future>
#include <iostream>
#include <iterator>
#include <ppltasks.h>
#include <thread>
#include <ctime>
#include <condition_variable>
#include <map>
#include <random>
#include <set>

#include "../unit_base/logging.h"

Client::Client(SimulcastAbility ability)
{
    simulcastAbility_ = ability;
}

SubscriptionList Client::setRssnrSubs(RssnrSubs subs)
{
    rssnrSubs_ = subs;
    subscriptionList_.clear();
    for (const auto& it : simulcastAbility_)
    {
        // update subscriptionList_
        const auto& feedbackList = rssnrSubs_.find(it.first);
        if (feedbackList != rssnrSubs_.end())
        {
            std::set<uint32_t> bitrateList;
            for (const auto& feedback : feedbackList->second)
            {
                const uint32_t bitrate = std::min(it.second.maxBitrateBps, 
                    std::max(feedback.targetKbps, it.second.minBitrateBps));
                bitrateList.insert(bitrate);
            }
            const uint32_t limitKbps = *bitrateList.rbegin() * svcRatio_;
            auto maxBitrateSuber = BitrateSuber{*bitrateList.rbegin(), 0};
            auto minBitrateSuber = BitrateSuber{ *bitrateList.begin(), 0};

            for (const auto& bitrate : bitrateList)
            {
                if (bitrate >= limitKbps || *bitrateList.begin() == *bitrateList.rbegin())
                {
                    ++maxBitrateSuber.count;
                }
                else
                {
                    ++minBitrateSuber.count;
                }
            }
            subscriptionList_[it.first] = SubSimulcastConfig{
                it.first, feedbackList->second.begin()->enable, maxBitrateSuber,
                minBitrateSuber, it.second.expectRateBps / 1000, 0
            };
        }
        else
        {
            subscriptionList_[it.first] = SubSimulcastConfig{
                it.first, false, BitrateSuber{}, BitrateSuber{},
                0, 0
            };
        }
    }
    return subscriptionList_;
}

SubscriptionList Client::hollyAllIn()
{
    auto fillBitrate = [this](SubscriptionList& subscriptionList)
    {
        for (auto& it : subscriptionList)
        {
            int needKbps = 0;
            if (!it.second.enable)
            {
                continue;
            }

            if (it.second.minBitrateSuer.bitrateKbps != it.second.maxBitrateSuber.bitrateKbps)
            {
                // has two bitrate in this layer
                const int maxKbps = static_cast<uint32_t>(it.second.minBitrateSuer.bitrateKbps / svcRatio_);
                needKbps = maxKbps - it.second.allocatedKbps;
            }
            else
            {
                needKbps = static_cast<int>(std::min(reservedBitrateKbps_, it.second.maxBitrateSuber.bitrateKbps)) - it.second.allocatedKbps;
            }

            it.second.allocatedKbps += needKbps;
            reservedBitrateKbps_ -= it.second.allocatedKbps;
            if (reservedBitrateKbps_ == 0)
            {
                break;
            }
        }
        return subscriptionList;
    };

    auto subscriptionList = subscriptionList_;
    reservedBitrateKbps_ = bweBitrateKbps_;
    uint32_t stashBitrateKbps = reservedBitrateKbps_;

    bool successful = true;

    // TODO: allocation by using minimum bitrate limit****************************************
    for (auto& it : subscriptionList)
    {
        if (!it.second.enable)
        {
            continue;
        }
        const auto needKbps = simulcastAbility_[it.first].minBitrateBps / 1000 - it.second.allocatedKbps;
        if (stashBitrateKbps >= needKbps)
        {
            it.second.allocatedKbps += needKbps;
            stashBitrateKbps -= needKbps;
        }
        else
        {
            successful = false;
            break;
        }
    }

    if (!successful)
    {
        // bwe is smaller than we needed minimum bitrate, allocate remaining bitrate
        return fillBitrate(subscriptionList);
    }

    // TODO: allocation by using [expect bitrate] limit*****************************************************
    // TODO: repetitive option, fix it
    // for (auto& it : subscriptionList)
    // {
    //     if (!it.second.enable)
    //     {
    //         continue;
    //     }
    //     const auto needKbps = std::min(it.second.minBitrateSuer.bitrateKbps,
    //                                    simulcastAbility_[it.first].expectRateBps / 1000) - it.second.allocatedKbps;
    //
    //     if (stashBitrateKbps >= needKbps && stashBitrateKbps)
    //     {
    //         it.second.allocatedKbps += needKbps;
    //         stashBitrateKbps -= needKbps;
    //     }
    //     else
    //     {
    //         successful = false;
    //         break;
    //     }
    // }
    //
    // if (!successful)
    // {
    //     BitrateSuber bitrateSuber;
    //     for (auto& it = subscriptionList.rbegin(); it != subscriptionList.rend(); ++it)
    //     {
    //         // save last higher layer
    //         if (bitrateSuber.count)
    //         {
    //             if (!it->second.enable)
    //             {
    //                 it->second.enable = true;
    //                 it->second.maxBitrateSuber = bitrateSuber;
    //                 it->second.minBitrateSuer.bitrateKbps = bitrateSuber.bitrateKbps;
    //             }
    //             else if (bitrateSuber.bitrateKbps > it->second.minBitrateSuer.bitrateKbps)
    //             {
    //                 it->second.maxBitrateSuber.bitrateKbps =
    //                     std::min(it->second.maxBitrateSuber.bitrateKbps, bitrateSuber.bitrateKbps);
    //                 it->second.maxBitrateSuber.count += bitrateSuber.count;
    //             }
    //             else
    //             {
    //                 it->second.minBitrateSuer.bitrateKbps =
    //                     std::min(it->second.minBitrateSuer.bitrateKbps, bitrateSuber.bitrateKbps);
    //                 it->second.minBitrateSuer.count += bitrateSuber.count;
    //             }
    //
    //             bitrateSuber = BitrateSuber{0, 0};
    //         }
    //
    //         if (it->second.minBitrateSuer.bitrateKbps != it->second.maxBitrateSuber.bitrateKbps
    //             && it->second.maxBitrateSuber.count > it->second.minBitrateSuer.count
    //             && it->second.minBitrateSuer.count != 0
    //             && it->first != simulcastAbility_.begin()->first
    //             && stashBitrateKbps > it->second.minBitrateSuer.bitrateKbps)
    //         {
    //             // two diff bitrate, change minimum to lower layer
    //             bitrateSuber = it->second.minBitrateSuer;
    //             it->second.minBitrateSuer = it->second.maxBitrateSuber;
    //             it->second.minBitrateSuer.count = 0;
    //         }
    //         else
    //         {
    //             // may has two diff bitrate, use minimum bitrate * svc_rate
    //             const int diffKbps = it->second.allocatedKbps -
    //                 static_cast<uint32_t>(it->second.minBitrateSuer.bitrateKbps / svcRatio_);
    //             if (diffKbps > 0)
    //             {
    //                 stashBitrateKbps += diffKbps;
    //                 it->second.allocatedKbps -= diffKbps;
    //             }
    //         }
    //     }
    //     return fillBitrate(subscriptionList);
    // }

    //allocation all bitrate limit
    for (auto& it : subscriptionList)
    {
        if (!it.second.enable)
        {
            continue;
        }
        const auto needKbps = it.second.maxBitrateSuber.bitrateKbps - it.second.allocatedKbps;

        if (stashBitrateKbps >= needKbps)
        {
            it.second.allocatedKbps += needKbps;
            stashBitrateKbps -= needKbps;
        }
        else
        {
            break;
        }
    }

    if (true)
    {
        BitrateSuber bitrateSuber;
        for (auto& it = subscriptionList.rbegin(); it != subscriptionList.rend(); ++it)
        {
            if (bitrateSuber.count)
            {
                if (!it->second.enable)
                {
                    it->second.enable = true;
                    it->second.maxBitrateSuber = bitrateSuber;
                    it->second.minBitrateSuer.bitrateKbps = bitrateSuber.bitrateKbps;
                }
                else if (bitrateSuber.bitrateKbps > it->second.minBitrateSuer.bitrateKbps)
                {
                    it->second.maxBitrateSuber.bitrateKbps =
                        std::min(it->second.maxBitrateSuber.bitrateKbps, bitrateSuber.bitrateKbps);
                    it->second.maxBitrateSuber.count += bitrateSuber.count;
                }
                else
                {
                    it->second.minBitrateSuer.bitrateKbps =
                        std::min(it->second.minBitrateSuer.bitrateKbps, bitrateSuber.bitrateKbps);
                    it->second.minBitrateSuer.count += bitrateSuber.count;
                }

                bitrateSuber = BitrateSuber{0, 0};
            }
            if (it->second.minBitrateSuer.bitrateKbps != it->second.maxBitrateSuber.bitrateKbps
                && it->second.maxBitrateSuber.count >= it->second.minBitrateSuer.count
                && it->second.minBitrateSuer.count != 0
                && it->first != simulcastAbility_.begin()->first
                && stashBitrateKbps > it->second.minBitrateSuer.bitrateKbps)
            {
                // two diff bitrate, change minimum to lower layer
                bitrateSuber = it->second.minBitrateSuer;
                it->second.minBitrateSuer = it->second.maxBitrateSuber;
                it->second.minBitrateSuer.count = 0;
            }
            else
            {
                // may has two diff bitrate, use minimum bitrate * svc_rate
                const int diffKbps = it->second.allocatedKbps -
                    static_cast<uint32_t>(it->second.minBitrateSuer.bitrateKbps / svcRatio_);
                if (diffKbps > 0)
                {
                    stashBitrateKbps += diffKbps;
                    it->second.allocatedKbps -= diffKbps;
                }
            }
        }
        return fillBitrate(subscriptionList);
    }
}

void Client::setSvcRatio(float ratio)
{
    svcRatio_ = std::min(1.0f, std::max(0.5f, ratio));
}

SubscribeSubs Client::calculateSubscribeSubs()
{
    SubscribeSubs subs;
    auto rssnrSubs = rssnrSubs_;
    std::set<uint32_t> lastBitrateList;
    for (auto& it = simulcastAbility_.rbegin(); it != simulcastAbility_.rend(); ++it)
    {
        const auto& feedbackList = rssnrSubs.find(it->first);
        if (feedbackList == rssnrSubs.end())
        {
            continue;
        }
        std::set<uint32_t> bitrateList = lastBitrateList;
        lastBitrateList.clear();
        // find bitrate which is enable
        for (const auto& feedback : feedbackList->second)
        {
            if (feedback.enable)
                bitrateList.insert(feedback.targetKbps);
        }

        if (bitrateList.empty())
        {
            continue;
        }

        const uint32_t min_bitrate_sub = std::max(*bitrateList.begin(), it->second.minBitrateBps);
        const uint32_t max_bitrate_sub = std::min(*bitrateList.rbegin(), it->second.maxBitrateBps);
        if (feedbackList->first == simulcastAbility_.begin()->first)
        {
            //in minimum mbps, can not move subs to lower simulcast layer
            subs[it->first] = SubscribeInfo{
                true, static_cast<uint32_t>(*bitrateList.begin() / svcRatio_),
                0
            };
        }
        else if (min_bitrate_sub > max_bitrate_sub * svcRatio_)
        {
            //in svc ability, cover suitable subs
            subs[it->first] = SubscribeInfo{
                true, max_bitrate_sub, 0
            };
        }
        else
        {
            //move the unrecoverable subs to lower simulcast layer

            const uint32_t limitKbps = max_bitrate_sub * svcRatio_;
            for (const auto& feedIter : feedbackList->second)
            {
                if (feedIter.targetKbps < limitKbps)
                {
                    lastBitrateList.insert(std::max(feedIter.targetKbps, it->second.minBitrateBps));
                }
            }
        }
    }
    return subs;
}

uint32_t Client::calculateUprightBitrateDiff(SubscribeSubs subs)
{
    return uint32_t();
}

void Client::setBweBitrate(uint32_t bitrateKbps)
{
    bweBitrateKbps_ = std::min<uint32_t>(40000, std::max<uint32_t>(bitrateKbps, 0));
}

SubscribeSubs Client::TryAllocateBweBitrate(SubscribeSubs subscribeSubs)
{
    uint32_t reservedKbps = bweBitrateKbps_;

    // first allocate, allocate max(minBitrate, neededBitrate) bitrate for every simulcast
    for (auto& it : subscribeSubs)
    {
        if (simulcastAbility_.find(it.first) == simulcastAbility_.end())
        {
            continue;
        }
        uint32_t layerAllocateKbps = 0;
        uint32_t needBitrate = simulcastAbility_[it.first].minBitrateBps - it.second.allocatedBitrateKbps;
        uint32_t minNeededBitrate = simulcastAbility_[it.first].minBitrateBps / 1000 - it.second.allocatedBitrateKbps;
        if (needBitrate > 0 && it.second.enable)
        {
            if (reservedKbps > needBitrate)
            {
                layerAllocateKbps = needBitrate;
            }
            else if (reservedKbps > minNeededBitrate)
            {
                layerAllocateKbps = minNeededBitrate;
            }
            it.second.allocatedBitrateKbps += layerAllocateKbps;
            reservedKbps -= layerAllocateKbps;
        }
    }

    for (auto& it : subscribeSubs)
    {
        if (simulcastAbility_.find(it.first) == simulcastAbility_.end()
            || it.second.allocatedBitrateKbps == it.second.neededBitrateKbps)
        {
            continue;
        }
        uint32_t layerAllocateKbps = 0;
        uint32_t neededBitrate = it.second.neededBitrateKbps - it.second.allocatedBitrateKbps;
        if (neededBitrate > 0 && it.second.enable)
        {
            if (reservedKbps < neededBitrate)
            {
                layerAllocateKbps = reservedKbps;
            }
            else
            {
                layerAllocateKbps = neededBitrate;
            }
            it.second.allocatedBitrateKbps += layerAllocateKbps;
            reservedKbps -= layerAllocateKbps;
        }

        if (reservedKbps == 0)
        {
            break;
        }
    }
    PrintSubscribeSubs(subscribeSubs, bweBitrateKbps_, reservedKbps);
    return subscribeSubs;
}

RssnrSubs Client::AdjustSubscription()
{
    return RssnrSubs();
}

uint32_t Client::getAllocateBitrateDiff() const
{
    return uint32_t();
}

void Client::PrintSubscribeSubs(SubscribeSubs subs, uint32_t bweKbps, uint32_t reservedKbps) const
{
    std::stringstream ss;
    ss << "bwe:" << bweKbps << "kbps" << ", reserve:" << reservedKbps << "kbps ";
    for (const auto& it : subs)
    {
        if (it.second.enable == false)
        {
            continue;
        }
        ss << it.first << "p[";
        ss << "enable:" << it.second.enable;
        ss << ", needed:" << it.second.neededBitrateKbps << "kbps";
        ss << ", allocated:" << it.second.allocatedBitrateKbps << "kbps] ";
    }
    LOG_MING << ss.str();
}

SubscriptionList Client::calculateAll(SubscriptionList subList, uint32_t bweKbps)
{
    /*
     *input: subList, bwe bitrate
     *
     *output: AllocationList
     */

    auto firstAllocationOut = subList;
    bool isFull = tryAllocation(firstAllocationOut, bweKbps);
    auto adjustSubsOut = firstAllocationOut;
    bool isAdjust = tryAdjustSubscription(adjustSubsOut);

    auto firstReservedKbps = allocateBitrate(firstAllocationOut, bweKbps);
    auto adjustReservedKbps = allocateBitrate(adjustSubsOut, bweKbps);

    //todo: can fix the logic
    if (isFull && firstReservedKbps == adjustReservedKbps)
    {
        reservedBitrateKbps_ = adjustReservedKbps;
        return adjustSubsOut;
    }
    else if (firstReservedKbps < adjustReservedKbps)
    {
        reservedBitrateKbps_ = firstReservedKbps;
        return firstAllocationOut;
    }
    else
    {
        reservedBitrateKbps_ = adjustReservedKbps;
        return adjustSubsOut;
    }
}

uint32_t Client::allocateBitrate(SubscriptionList& subs, uint32_t sumOfBitrateKbps)
{
    for (auto& it : subs)
    {
        if (it.second.enable)
        {
            const auto& info = it.second;
            int additonKbps = 0;
            if (sumOfBitrateKbps < it.second.allocatedKbps)
            {
                LOG_MING << "warning, sum: " << sumOfBitrateKbps << "< all: " << it.second.allocatedKbps;
                it.second.allocatedKbps = 0;
            }
            sumOfBitrateKbps -= it.second.allocatedKbps;
            if (info.minBitrateSuer.bitrateKbps == info.maxBitrateSuber.bitrateKbps)
            {
                // one bitrate in this layer
                additonKbps = info.minBitrateSuer.bitrateKbps;
            }
            else
            {
                // two diff bitrate in this layer
                additonKbps = std::min<uint32_t>(info.minBitrateSuer.bitrateKbps / svcRatio_,
                                                 info.maxBitrateSuber.bitrateKbps);
            }
            additonKbps -= it.second.allocatedKbps;

            if (additonKbps > sumOfBitrateKbps)
            {
                if((sumOfBitrateKbps + info.allocatedKbps) > simulcastAbility_[info.mbps].minBitrateBps / 1000)
                {
                    additonKbps = sumOfBitrateKbps;
                    it.second.allocatedKbps += additonKbps;
                    sumOfBitrateKbps -= additonKbps;
                }
                break;
            }
            else
            {
                it.second.allocatedKbps += additonKbps;
                sumOfBitrateKbps -= additonKbps;
            }
        }
    }
    return sumOfBitrateKbps;
}

bool Client::tryAllocation(SubscriptionList& subs, uint32_t bitrateKbps)
{
    bool hasSubscribe = false;
    for (int i = 0; i < 3; ++i)
    {
        for (auto& it : subs)
        {
            if (it.second.enable)
            {
                hasSubscribe = true;
                auto& info = it.second;
                uint32_t additonKbps = 0;
                uint32_t nowMaxLimitKbps = 0;

                // check if is two bitrate in this layer
                const uint32_t pointKbps = info.minBitrateSuer.count
                                               ? static_cast<uint32_t>(info.minBitrateSuer.bitrateKbps / svcRatio_)
                                               : info.maxBitrateSuber.bitrateKbps;

                if (info.allocatedKbps < simulcastAbility_[it.first].minBitrateBps / 1000)
                {
                    // allocate by using min_bitrate
                    nowMaxLimitKbps = simulcastAbility_[it.first].minBitrateBps / 1000;
                }
                else if (info.allocatedKbps < simulcastAbility_[it.first].expectRateBps / 1000)
                {
                    nowMaxLimitKbps = simulcastAbility_[it.first].expectRateBps / 1000;
                    // allocate by using expect_bitrate
                }
                else
                {
                    nowMaxLimitKbps = pointKbps;
                }

                nowMaxLimitKbps = std::min(pointKbps, nowMaxLimitKbps);
                if (info.allocatedKbps < pointKbps)
                {
                    additonKbps = nowMaxLimitKbps - info.allocatedKbps;
                    if (bitrateKbps >= additonKbps)
                    {
                        bitrateKbps -= additonKbps;
                        info.allocatedKbps += additonKbps;
                    }
                    else
                    {
                        return false;
                    }
                }
            }
        }
        if (!hasSubscribe)
            return false;
    }
    return true;
}

bool Client::tryAdjustSubscription(SubscriptionList& subs)
{
    bool changed = false;
    BitrateSuber lastLayerSub;
    for (auto& it = subs.rbegin(); it != subs.rend(); ++it)
    {
        auto& info = it->second;
        if (lastLayerSub.count && lastLayerSub.bitrateKbps)
        {
            // <bitrate, count>
            info.enable = true;
            std::map<uint32_t, uint32_t> stashMap;
            const std::vector<BitrateSuber> suberList{lastLayerSub, info.minBitrateSuer, info.maxBitrateSuber};
            for (const auto& it : suberList)
            {
                if (it.count)
                {
                    if (stashMap.find(it.bitrateKbps) == stashMap.end())
                    {
                        stashMap[it.bitrateKbps] = it.count;
                    }
                    else
                    {
                        stashMap[it.bitrateKbps] += it.count;
                    }
                }
            }

            const uint32_t limitKbps = std::min(simulcastAbility_[info.mbps].maxBitrateBps / 1000, 
                stashMap.rbegin()->first) * svcRatio_;
            const uint32_t maxKbps = simulcastAbility_[info.mbps].maxBitrateBps / 1000;
            auto maxBitrateSuber = BitrateSuber{
                std::min(stashMap.rbegin()->first, maxKbps), 0
            };
            auto minBitrateSuber = BitrateSuber{
                std::min(maxKbps, stashMap.begin()->first), 0
            };

            for (const auto& suber : stashMap)
            {
                if (suber.first >= limitKbps || stashMap.begin()->first == stashMap.rbegin()->first)
                {
                    maxBitrateSuber.count += suber.second;
                }
                else
                {
                    minBitrateSuber.count += suber.second;
                }
            }

            info.maxBitrateSuber = maxBitrateSuber;
            info.minBitrateSuer = minBitrateSuber;
            // reset lastLayerSub
            lastLayerSub = BitrateSuber();
        }

        if (info.minBitrateSuer.count
            && info.minBitrateSuer.count <= info.maxBitrateSuber.count
            && info.mbps != simulcastAbility_.begin()->first
            && info.enable)
        {
            lastLayerSub = info.minBitrateSuer;
            info.minBitrateSuer.bitrateKbps = info.maxBitrateSuber.bitrateKbps;
            info.minBitrateSuer.count = 0;
            changed = true;
        }
    }
    return changed;
}

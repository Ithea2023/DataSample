#include "logging.h"

#include <chrono>
#include <ostream>
#include <iomanip>
#include <iostream>
#include <Windows.h>

const  bool Win_Thread = true;
const  bool Time_Enable = true;
typedef DWORD PlatformThreadId;

const char* FilenameFromPath(const char* file) {
  const char* end1 = ::strrchr(file, '/');
  const char* end2 = ::strrchr(file, '\\');
  if (!end1 && !end2)
    return file;
  else
    return (end1 > end2) ? end1 + 1 : end2 + 1;
}

LogStream::LogStream(const char * file_name, const int line):file_name_(file_name),line_(line)
{
  if(Win_Thread) {
    PlatformThreadId id = GetCurrentThreadId();
    print_stream_ << "" << std::dec << id << ": ";
  }
}

LogStream::LogStream(const char * file_name, const int line, const char* mark) :file_name_(file_name), line_(line)
{
  if (Win_Thread) {
    PlatformThreadId id = GetCurrentThreadId();
    print_stream_ << "" << std::dec << id << ": " << mark << " ";
  }
}

LogStream::~LogStream()
{
  print_stream_ << " (" << FilenameFromPath(file_name_) << ":" << line_ << ") ";
  std::cout << "\n" << print_stream_.str();
}


//
//
//
// {
//
//
//     SubscriptionList Client::hollyAllIn()
//     {
//         auto fillBitrate = [this](SubscriptionList& subscriptionList)
//         {
//             for (auto& it : subscriptionList)
//             {
//                 int needKbps = 0;
//                 if (!it.second.enable)
//                 {
//                     continue;
//                 }
//
//                 if (it.second.minBitrateSuer.bitrateKbps != it.second.maxBitrateSuber.bitrateKbps)
//                 {
//                     // has two bitrate in this layer
//                     const int maxKbps = static_cast<uint32_t>(it.second.minBitrateSuer.bitrateKbps / svcRatio_);
//                     needKbps = maxKbps - it.second.allocatedKbps;
//                 }
//                 else
//                 {
//                     needKbps = static_cast<int>(std::min(reservedBitrateKbps_, it.second.maxBitrateSuber.bitrateKbps)) - it.second.allocatedKbps;
//                 }
//
//                 it.second.allocatedKbps += needKbps;
//                 reservedBitrateKbps_ -= it.second.allocatedKbps;
//                 if (reservedBitrateKbps_ == 0)
//                 {
//                     break;
//                 }
//             }
//             return subscriptionList;
//         };
//
//         auto subscriptionList = subscriptionList_;
//         reservedBitrateKbps_ = bweBitrateKbps_;
//         uint32_t stashBitrateKbps = reservedBitrateKbps_;
//
//         bool successful = true;
//
//         // TODO: allocation by using minimum bitrate limit****************************************
//         for (auto& it : subscriptionList)
//         {
//             if (!it.second.enable)
//             {
//                 continue;
//             }
//             const auto needKbps = simulcastAbility_[it.first].minBitrateBps / 1000 - it.second.allocatedKbps;
//             if (stashBitrateKbps >= needKbps)
//             {
//                 it.second.allocatedKbps += needKbps;
//                 stashBitrateKbps -= needKbps;
//             }
//             else
//             {
//                 successful = false;
//                 break;
//             }
//         }
//
//         if (!successful)
//         {
//             // bwe is smaller than we needed minimum bitrate, allocate remaining bitrate
//             return fillBitrate(subscriptionList);
//         }
//
//         // TODO: allocation by using [expect bitrate] limit*****************************************************
//         // TODO: repetitive option, fix it
//         // for (auto& it : subscriptionList)
//         // {
//         //     if (!it.second.enable)
//         //     {
//         //         continue;
//         //     }
//         //     const auto needKbps = std::min(it.second.minBitrateSuer.bitrateKbps,
//         //                                    simulcastAbility_[it.first].expectRateBps / 1000) - it.second.allocatedKbps;
//         //
//         //     if (stashBitrateKbps >= needKbps && stashBitrateKbps)
//         //     {
//         //         it.second.allocatedKbps += needKbps;
//         //         stashBitrateKbps -= needKbps;
//         //     }
//         //     else
//         //     {
//         //         successful = false;
//         //         break;
//         //     }
//         // }
//         //
//         // if (!successful)
//         // {
//         //     BitrateSuber bitrateSuber;
//         //     for (auto& it = subscriptionList.rbegin(); it != subscriptionList.rend(); ++it)
//         //     {
//         //         // save last higher layer
//         //         if (bitrateSuber.count)
//         //         {
//         //             if (!it->second.enable)
//         //             {
//         //                 it->second.enable = true;
//         //                 it->second.maxBitrateSuber = bitrateSuber;
//         //                 it->second.minBitrateSuer.bitrateKbps = bitrateSuber.bitrateKbps;
//         //             }
//         //             else if (bitrateSuber.bitrateKbps > it->second.minBitrateSuer.bitrateKbps)
//         //             {
//         //                 it->second.maxBitrateSuber.bitrateKbps =
//         //                     std::min(it->second.maxBitrateSuber.bitrateKbps, bitrateSuber.bitrateKbps);
//         //                 it->second.maxBitrateSuber.count += bitrateSuber.count;
//         //             }
//         //             else
//         //             {
//         //                 it->second.minBitrateSuer.bitrateKbps =
//         //                     std::min(it->second.minBitrateSuer.bitrateKbps, bitrateSuber.bitrateKbps);
//         //                 it->second.minBitrateSuer.count += bitrateSuber.count;
//         //             }
//         //
//         //             bitrateSuber = BitrateSuber{0, 0};
//         //         }
//         //
//         //         if (it->second.minBitrateSuer.bitrateKbps != it->second.maxBitrateSuber.bitrateKbps
//         //             && it->second.maxBitrateSuber.count > it->second.minBitrateSuer.count
//         //             && it->second.minBitrateSuer.count != 0
//         //             && it->first != simulcastAbility_.begin()->first
//         //             && stashBitrateKbps > it->second.minBitrateSuer.bitrateKbps)
//         //         {
//         //             // two diff bitrate, change minimum to lower layer
//         //             bitrateSuber = it->second.minBitrateSuer;
//         //             it->second.minBitrateSuer = it->second.maxBitrateSuber;
//         //             it->second.minBitrateSuer.count = 0;
//         //         }
//         //         else
//         //         {
//         //             // may has two diff bitrate, use minimum bitrate * svc_rate
//         //             const int diffKbps = it->second.allocatedKbps -
//         //                 static_cast<uint32_t>(it->second.minBitrateSuer.bitrateKbps / svcRatio_);
//         //             if (diffKbps > 0)
//         //             {
//         //                 stashBitrateKbps += diffKbps;
//         //                 it->second.allocatedKbps -= diffKbps;
//         //             }
//         //         }
//         //     }
//         //     return fillBitrate(subscriptionList);
//         // }
//
//         //allocation all bitrate limit
//         for (auto& it : subscriptionList)
//         {
//             if (!it.second.enable)
//             {
//                 continue;
//             }
//             const auto needKbps = it.second.maxBitrateSuber.bitrateKbps - it.second.allocatedKbps;
//
//             if (stashBitrateKbps >= needKbps)
//             {
//                 it.second.allocatedKbps += needKbps;
//                 stashBitrateKbps -= needKbps;
//             }
//             else
//             {
//                 break;
//             }
//         }
//
//         if (true)
//         {
//             BitrateSuber bitrateSuber;
//             for (auto& it = subscriptionList.rbegin(); it != subscriptionList.rend(); ++it)
//             {
//                 if (bitrateSuber.count)
//                 {
//                     if (!it->second.enable)
//                     {
//                         it->second.enable = true;
//                         it->second.maxBitrateSuber = bitrateSuber;
//                         it->second.minBitrateSuer.bitrateKbps = bitrateSuber.bitrateKbps;
//                     }
//                     else if (bitrateSuber.bitrateKbps > it->second.minBitrateSuer.bitrateKbps)
//                     {
//                         it->second.maxBitrateSuber.bitrateKbps =
//                             std::min(it->second.maxBitrateSuber.bitrateKbps, bitrateSuber.bitrateKbps);
//                         it->second.maxBitrateSuber.count += bitrateSuber.count;
//                     }
//                     else
//                     {
//                         it->second.minBitrateSuer.bitrateKbps =
//                             std::min(it->second.minBitrateSuer.bitrateKbps, bitrateSuber.bitrateKbps);
//                         it->second.minBitrateSuer.count += bitrateSuber.count;
//                     }
//
//                     bitrateSuber = BitrateSuber{ 0, 0 };
//                 }
//                 if (it->second.minBitrateSuer.bitrateKbps != it->second.maxBitrateSuber.bitrateKbps
//                     && it->second.maxBitrateSuber.count >= it->second.minBitrateSuer.count
//                     && it->second.minBitrateSuer.count != 0
//                     && it->first != simulcastAbility_.begin()->first
//                     && stashBitrateKbps > it->second.minBitrateSuer.bitrateKbps)
//                 {
//                     // two diff bitrate, change minimum to lower layer
//                     bitrateSuber = it->second.minBitrateSuer;
//                     it->second.minBitrateSuer = it->second.maxBitrateSuber;
//                     it->second.minBitrateSuer.count = 0;
//                 }
//                 else
//                 {
//                     // may has two diff bitrate, use minimum bitrate * svc_rate
//                     const int diffKbps = it->second.allocatedKbps -
//                         static_cast<uint32_t>(it->second.minBitrateSuer.bitrateKbps / svcRatio_);
//                     if (diffKbps > 0)
//                     {
//                         stashBitrateKbps += diffKbps;
//                         it->second.allocatedKbps -= diffKbps;
//                     }
//                 }
//             }
//             return fillBitrate(subscriptionList);
//         }
//     }
//
//
//
//
//
//
//
//
//
//
//
// }
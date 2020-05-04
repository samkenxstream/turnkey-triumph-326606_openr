/**
 * Copyright (c) 2014-present, Facebook, Inc.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <list>

#include <fbzmq/async/ZmqEventLoop.h>

#include <openr/nl/NetlinkProtocolSocket.h>

namespace openr::fbnl {

/**
 * Utility functions for creating objects in tests
 */
namespace utils {

fbnl::Link createLink(
    const int ifIndex,
    const std::string& ifName,
    bool isUp = true,
    bool isLoopback = false);

fbnl::IfAddress createIfAddress(const int ifIndex, const std::string& addrMask);

} // namespace utils

/**
 * Defines a fake implementation for netlink protocol socket. Instead of writing
 * state to Linux kernel, the API calls made here instead read/write into the
 * state maintained in memory. There are also specialized APIs to update the
 * state.
 *
 * This class facilitates testing of application logic with unit-tests.
 */
class FakeNetlinkProtocolSocket : public NetlinkProtocolSocket {
 public:
  explicit FakeNetlinkProtocolSocket(fbzmq::ZmqEventLoop* evl)
      : NetlinkProtocolSocket(evl) {}

  /**
   * API to create links for testing purposes
   */
  folly::SemiFuture<int> addLink(const fbnl::Link& link);

  /**
   * Overrides API of NetlinkProtocolSocket for testing
   */

  folly::SemiFuture<int> addRoute(const fbnl::Route& route) override;
  folly::SemiFuture<int> deleteRoute(const fbnl::Route& route) override;
  folly::SemiFuture<std::vector<fbnl::Route>> getRoutes(
      const fbnl::Route& filter) override;

  folly::SemiFuture<int> addIfAddress(const fbnl::IfAddress&) override;
  folly::SemiFuture<int> deleteIfAddress(const fbnl::IfAddress&) override;
  folly::SemiFuture<std::vector<fbnl::IfAddress>> getAllIfAddresses() override;

  folly::SemiFuture<std::vector<fbnl::Link>> getAllLinks() override;

  folly::SemiFuture<std::vector<fbnl::Neighbor>> getAllNeighbors() override;

 protected:
  void
  init() override {
    // empty
  }

 private:
  // map<ifIndex -> Link>
  // NOTE: using map for ordered entries
  std::map<int, fbnl::Link> links_;

  // map<ifIndex -> list<IfAddress>>
  // NOTE: using map for ordered entries
  std::map<int, std::list<fbnl::IfAddress>> ifAddrs_;

  // map<protocolId -> map<prefix/label, Route>
  // NOTE: using map for ordered entries
  std::unordered_map<uint8_t, std::map<folly::CIDRNetwork, fbnl::Route>>
      unicastRoutes_;
  std::unordered_map<uint8_t, std::map<uint32_t, fbnl::Route>> mplsRoutes_;
};

} // namespace openr::fbnl

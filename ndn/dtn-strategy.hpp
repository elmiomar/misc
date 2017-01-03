#ifndef NFD_DAEMON_FW_DTN_STRATEGY_HPP
#define NFD_DAEMON_FW_DTN_STRATEGY_HPP

#include "strategy.hpp"
#include "best-route-strategy.hpp"
#include <chrono>
#include <thread>
#include <time.h>
#include <ndn-cxx/face.hpp>

namespace nfd {
namespace fw {

/** \brief a forwarding strategy that forwards Interest to all FIB nexthops
 */
class DtnStrategy : public BestRouteStrategy
{
public:
  DtnStrategy(Forwarder& forwarder, const Name& name = STRATEGY_NAME);

  virtual void
  afterReceiveInterest(const Face& inFace, const Interest& interest, const shared_ptr<pit::Entry>& pitEntry) override;

  virtual void
  afterReceiveNack(const Face& inFace, const lp::Nack& nack, const shared_ptr<pit::Entry>& pitEntry) override;

  virtual void
  beforeSatisfyInterest(const shared_ptr<pit::Entry>& pitEntry, const Face& inFace, const Data& data) override;

  virtual void
  beforeExpirePendingInterest(const shared_ptr<pit::Entry>& pitEntry) override;

private:
  void
  createNewPitEntry(const shared_ptr<pit::Entry>& pitEntry);
private:
  void
  onData(const Interest& interest, const Data& data);

  void
  onNack(const Interest& interest, const lp::Nack& nack);

  void
  onTimeout(const Interest& interest);

  uint32_t
  getNewNonce();

public:
  static const Name STRATEGY_NAME;

private:
  Pit& m_pit;
  Forwarder& m_forwarder;
  ndn::Face m_face;
  std::vector<uint32_t> m_nonces;
};

} // namespace fw
} // namespace nfd

#endif // NFD_DAEMON_FW_DTN_STRATEGY_HPP

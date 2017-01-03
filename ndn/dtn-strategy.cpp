#include "dtn-strategy.hpp"
#include "pit-algorithm.hpp"

namespace nfd {
namespace fw {

NFD_LOG_INIT("DtnStrategy");

const Name DtnStrategy::STRATEGY_NAME("ndn:/localhost/nfd/strategy/dtn/%FD%01");
NFD_REGISTER_STRATEGY(DtnStrategy);

DtnStrategy::DtnStrategy(Forwarder& forwarder, const Name& name)
  : BestRouteStrategy(forwarder, name)
  , m_pit(forwarder.getPit())
  , m_forwarder(forwarder)
{
  //srand(time(NULL));
}

void
DtnStrategy::afterReceiveInterest(const Face& inFace, const Interest& interest, const shared_ptr<pit::Entry>& pitEntry)
{
  if (hasPendingOutRecords(*pitEntry)) {
    // not a new Interest, don't forward
    return;
  }

  const fib::Entry& fibEntry = this->lookupFib(*pitEntry);
  const fib::NextHopList& nexthops = fibEntry.getNextHops();
  fib::NextHopList::const_iterator it = std::find_if(nexthops.begin(), nexthops.end(),
    [&pitEntry] (const fib::NextHop& nexthop) { return canForwardToLegacy(*pitEntry, nexthop.getFace()); });

  if (it == nexthops.end()) {
    NFD_LOG_DEBUG("No Nexthops");
    this->rejectPendingInterest(pitEntry);
    return;
  }

  Face& outFace = it->getFace();
  this->sendInterest(pitEntry, outFace);
}

void
DtnStrategy::afterReceiveNack(const Face& inFace, const lp::Nack& nack, const shared_ptr<pit::Entry>& pitEntry)
{
  return;
}

void
DtnStrategy::beforeSatisfyInterest(const shared_ptr<pit::Entry>& pitEntry, const Face& inFace, const Data& data)
{
 // do nothing
}

uint32_t
DtnStrategy::getNewNonce()
{

  if (m_nonces.size() > 1000)
    m_nonces.clear();

  uint32_t randomNonce = static_cast<uint32_t>(std::rand());
  while (std::find(m_nonces.begin(), m_nonces.end(), randomNonce) != m_nonces.end())
    randomNonce = static_cast<uint32_t>(std::rand());

  m_nonces.push_back(randomNonce);

  return randomNonce;
}

void
DtnStrategy::beforeExpirePendingInterest(const shared_ptr<pit::Entry>& pitEntry)
{
  Interest interest = pitEntry->getInterest();
  //interest.setInterestLifetime(time::milliseconds(interest.getInterestLifetime().count()));
  //interest.setMustBeFresh(true);
  interest.setNonce(getNewNonce());
  NFD_LOG_DEBUG("beforeExpirePendingInterest: Sending Interest: " << interest);
  m_face.expressInterest(interest,
                          bind(&DtnStrategy::onData, this,  _1, _2),
                          bind(&DtnStrategy::onNack, this, _1, _2),
                          bind(&DtnStrategy::onTimeout, this, _1));
  m_face.processEvents();
}

void
DtnStrategy::onData(const Interest& interest, const Data& data)
{
  NFD_LOG_DEBUG("Data: " << data);
}

void
DtnStrategy::onNack(const Interest& interest, const lp::Nack& nack)
{
  NFD_LOG_DEBUG("Nack: " << interest);
}

void
DtnStrategy::onTimeout(const Interest& interest)
{
  NFD_LOG_DEBUG("Timeout: " << interest);
}

// void
// DtnStrategy::createNewPitEntry(const shared_ptr<pit::Entry>& pitEntry)
// {
//   NFD_LOG_DEBUG("Pit size before: " << m_forwarder.getPit().size());
//   //extract name to trace
//   Interest interest = pitEntry->getInterest();
//   Name name = interest.getName();
//   uint32_t nonce = interest.getNonce();

//   // create interest using extracted name
//   shared_ptr<Interest> newInterest = make_shared<Interest>(name);
//   newInterest->setNonce(nonce);
//   newInterest->setInterestLifetime(time::milliseconds(interest.getInterestLifetime()));
//   NFD_LOG_DEBUG("Created new Interest: " << *newInterest << " from old one: "<< pitEntry->getInterest());

//   shared_ptr<pit::Entry> newPitEntry = m_forwarder.getPit().insert(*newInterest).first;
//   NFD_LOG_DEBUG("Pit size after: " << m_forwarder.getPit().size());
// }

} // namespace fw
} // namespace nfd

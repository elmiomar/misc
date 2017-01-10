#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/filesystem.hpp>
#include <boost/noncopyable.hpp>

#include "face.hpp"

// Enclosing code in ndn simplifies coding (can also use `using namespace ndn`)
namespace ndn {
// Additional nested namespace could be used to prevent/limit name contentions
namespace examples {

class VConsumer : noncopyable
{
public:
  explicit
  VConsumer()
  : m_face(m_ioService)
  , m_hasError(false)
  {
  }

void
run()
{
  boost::asio::signal_set signalSet(m_ioService, SIGINT, SIGTERM);
  signalSet.async_wait(bind(&VConsumer::signalHandler, this));

  boost::asio::deadline_timer timer(m_ioService, boost::posix_time::millisec(0));
  timer.async_wait(bind(&VConsumer::sendInterest, this));
  try
  {
    m_face.processEvents();
  }
  catch (const std::exception& e) {
    m_hasError = true;
    m_ioService.stop();
  }


}

void
sendInterest()
{
  Interest interest(Name("/sn/test"));
  interest.setInterestLifetime(time::milliseconds(3000));
  interest.setMustBeFresh(false);

  m_face.expressInterest(interest,
                         bind(&VConsumer::onData, this,  _1, _2),
                         bind(&VConsumer::onNack, this, _1, _2),
                         bind(&VConsumer::onTimeout, this, _1));

  std::cout << "Sending " << interest << std::endl;
  
}

void
signalHandler()
{

 m_face.shutdown();
 m_ioService.stop();

 exit(m_hasError ? EXIT_FAILURE : EXIT_SUCCESS);
}

private:
  void
  onData(const Interest& interest, const Data& data)
  {
    std::cout << data << std::endl;
    // m_hasError = false;
    // signalHandler();

  }

  void
  onNack(const Interest& interest, const lp::Nack& nack)
  {
    std::cout << "Nack: Interest " << interest << "\nReason = " << nack.getReason() <<  std::endl;
    // m_hasError = true;
    // boost::asio::deadline_timer timer(m_ioService, boost::posix_time::millisec(interest.getInterestLifetime().count()));
    // timer.wait();
    // sendInterest();
  }

  void
  onTimeout(const Interest& interest)
  {
    std::cout << "Timeout: " << interest << std::endl;
    m_hasError = true;
    // sendInterest();
  }

private:
  boost::asio::io_service m_ioService;
  Face m_face;
  bool m_hasError;
};

} // namespace examples
} // namespace ndn

int
main(int argc, char** argv)
{
  ndn::examples::VConsumer c;
  try {
    c.run();
  }
  catch (const std::exception& e) {
    std::cerr << "ERROR: " << e.what() << std::endl;
  }
  return 0;
}

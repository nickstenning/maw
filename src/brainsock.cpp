#include "zmq.hpp"

#include "brain.h"
#include "pendulum.h"
#include "util.h"

int main (int /*argc*/, char* const /*argv*/[]) {
  unsigned int seed = util::initRNG();
  std::cout << "# RNG_SEED = " << seed << "\n";

  Pendulum pendulum;
  Brain brain;

  brain << std::cin;

  zmq::context_t ctx(1);

  zmq::socket_t comm(m_ctx, ZMQ_REP);
  zmq::socket_t data(m_ctx, ZMQ_PUSH);

  comm.connect("tcp://127.0.0.1:5000");
  data.connect("tcp://127.0.0.1:5001");

  //  Initialize poll set
  zmq::pollitem_t poll_items [] = {
      { comm, 0, ZMQ_POLLIN, 0 }
  };

}




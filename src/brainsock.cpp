#include <sstream>
#include <unistd.h>
#include "zmq.h"

// #include "brain.h"
#include "pendulum.h"
#include "util.h"
#include "zhelpers.h"

int main (int /*argc*/, char* const /*argv*/[]) {
  unsigned int seed = util::initRNG();
  std::cout << "# RNG_SEED = " << seed << "\n";

  Pendulum pendulum;
  // Brain brain;

  // brain << std::cin;

  zmq::context_t ctx(1);

  zmq::socket_t comm(ctx, ZMQ_REP);
  zmq::socket_t data(ctx, ZMQ_REP);

  comm.connect("tcp://127.0.0.1:5000");
  data.connect("tcp://127.0.0.1:5001");

  // Initialize poll set
  zmq::pollitem_t poll_items [] = {
    { comm, 0, ZMQ_POLLIN, 0 },
    { data, 0, ZMQ_POLLIN, 0 }
  };

  // Initialize pendulum
  pendulum.ang(0.1);

  while (1) {
    zmq::poll(&poll_items[0], 2, -1);

    if (poll_items[0].revents & ZMQ_POLLIN) {
      std::string msg = s_recv(comm);
      std::cout << "Received event on comm: " << msg << "\n";
      s_send(comm, "Here's a comm reply");
    }

    if (poll_items[1].revents & ZMQ_POLLIN) {
      std::stringstream stream;
      stream << pendulum.ang();
      s_recv(data);
      s_send(data, stream.str());
    }

    pendulum.step(0.0);
  }

  return 0;

}




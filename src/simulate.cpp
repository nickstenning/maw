#include <sstream>
#include <vector>

#include "zmq.h"

#include "unicycle_2d.h"
#include "zhelpers.h"
#include "util.h"

int process_comm(zmq::socket_t&, Unicycle2D&);
int process_data(zmq::socket_t&, Unicycle2D&);

int main (int /*argc*/, char* const /*argv*/[]) {
  Unicycle2D uni;

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

  // Initialize unicycle;
  uni.p(util::rand(-0.001, 0.001));

  while (1) {
    zmq::poll(&poll_items[0], 2, -1);

    // Got command
    if (poll_items[0].revents & ZMQ_POLLIN) {
      process_comm(comm, uni);
    }

    // Got request for data
    if (poll_items[1].revents & ZMQ_POLLIN) {
      process_data(data, uni);
    }

    std::cout << uni.T() << " + " << uni.V() << " = " << uni.T() + uni.V() << "\n";
  }

  return 0;
}

int process_comm(zmq::socket_t& socket, Unicycle2D& uni) {
  std::istringstream s_in( s_recv(socket) );
  std::string cmd;

  s_in >> cmd;

  // Set random start position
  uni.p(util::rand(-1, 1));

  s_send(socket, "OK");

  return 0;
}

int process_data(zmq::socket_t& socket, Unicycle2D& uni) {
  std::istringstream s_in( s_recv(socket) );

  double dt;
  s_in >> dt;

  uni.step(dt);

  std::ostringstream s_out;
  s_out << uni.p() << " " << uni.w();

  s_send(socket, s_out.str());

  return 0;
}




#include <sstream>
#include <unistd.h>
#include "zmq.h"

// #include "brain.h"
#include "pendulum.h"
#include "util.h"
#include "zhelpers.h"

int process_comm(zmq::socket_t& socket, Pendulum& pendulum, double& torque);
int process_data(zmq::socket_t& socket, Pendulum& pendulum, double& torque);

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

  double torque = 0.0;

  while (1) {
    zmq::poll(&poll_items[0], 2, -1);

    // Got command
    if (poll_items[0].revents & ZMQ_POLLIN) {
      process_comm(comm, pendulum, torque);
    }

    // Got request for data
    if (poll_items[1].revents & ZMQ_POLLIN) {
      process_data(data, pendulum, torque);
    }
  }

  return 0;
}

int process_comm(zmq::socket_t& socket, Pendulum& /*pendulum*/, double& torque) {
  std::istringstream s_in( s_recv(socket) );
  std::string cmd;

  s_in >> cmd;

  if (cmd.compare("torque") != -1) {
    // Set torque from message body.
    s_in >> torque;
  }

  s_send(socket, "OK");

  return 0;
}

int process_data(zmq::socket_t& socket, Pendulum& pendulum, double& torque) {
  std::istringstream s_in( s_recv(socket) );
  double dt;

  s_in >> dt;

  double start = pendulum.time();
  while (pendulum.time() < start + dt) {
    pendulum.step(torque);
  }

  std::ostringstream s_out;
  s_out << pendulum.ang();
  s_send(socket, s_out.str());

  return 0;
}




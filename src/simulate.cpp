#include <sstream>
#include <vector>
#include <unistd.h>
#include "zmq.h"

#include "brain.h"
#include "pendulum.h"
#include "util.h"
#include "zhelpers.h"

#define USER_TORQUE       20.0
#define CONTROLLER_TORQUE 10.0

int process_comm(zmq::socket_t&, Pendulum&, Brain&, int& torqueDir);
int process_data(zmq::socket_t&, Pendulum&, Brain&, int& torqueDir);

int main (int /*argc*/, char* const /*argv*/[]) {
  util::initRNG();

  Pendulum pendulum;
  Brain brain;

  std::cin >> brain;

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
  pendulum.ang(util::rand(-3, 3));

  int torqueDir = 0;

  while (1) {
    zmq::poll(&poll_items[0], 2, -1);

    // Got command
    if (poll_items[0].revents & ZMQ_POLLIN) {
      process_comm(comm, pendulum, brain, torqueDir);
    }

    // Got request for data
    if (poll_items[1].revents & ZMQ_POLLIN) {
      process_data(data, pendulum, brain, torqueDir);
    }
  }

  return 0;
}

double getControlTorque(Pendulum& pendulum, Brain& brain) {
  std::vector<double> input;
  std::vector<int> output;

  input.push_back(pendulum.ang());
  input.push_back(pendulum.vel());

  output = brain.feedForward(input);

  double controlTorque = CONTROLLER_TORQUE * output[0];

  return controlTorque;
}

int process_comm(zmq::socket_t& socket, Pendulum& /*pendulum*/, Brain& /*brain*/, int& torqueDir) {
  std::istringstream s_in( s_recv(socket) );
  std::string cmd;

  s_in >> cmd;

  if (cmd.compare("torque") != -1) {
    // Set torque from message body.
    s_in >> torqueDir;
  }

  s_send(socket, "OK");

  return 0;
}

int process_data(zmq::socket_t& socket, Pendulum& pendulum, Brain& brain, int& torqueDir) {
  std::istringstream s_in( s_recv(socket) );

  double dt;
  s_in >> dt;

  double controlTorque = getControlTorque(pendulum, brain);

  pendulum.step(controlTorque + USER_TORQUE * torqueDir, dt);

  std::ostringstream s_out;
  s_out << pendulum.ang();
  s_send(socket, s_out.str());

  return 0;
}



